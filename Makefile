# commands
ASM := nasm
CAT := cat
CC := x86_64-elf-gcc
LD := x86_64-elf-ld
TRUN := truncate

# the architecture we build for, everything under kernel/arch/ is picked per architecture
ARCH := x86_64
ARCHDIR := kernel/arch/$(ARCH)

# how many 512 byte sectors stage 1 pulls off the disk for stage 2 + the kernel
STAGE2_SECTORS := 32
STAGE2_BYTES := $(shell expr $(STAGE2_SECTORS) \* 512)

# the bootloader lives in its own directory so it stays a self contained project we can update on its own
BOOTDIR := Doomboot
ST1  := $(BOOTDIR)/src/stage1.asm
ST2  := $(BOOTDIR)/src/stage2.asm
LINKER := $(BOOTDIR)/linker/linker.ld
INCDIR := $(BOOTDIR)/src/

# every c file in kernel/kernel/ is part of the kernel so adding a new one needs no change down here
KERN_SRC := $(wildcard kernel/kernel/*.c)
KERN_OBJ := $(patsubst kernel/%.c,build/%.o,$(KERN_SRC))

# the architecture specific part of the kernel (tty, ports, gdt, ...), same deal, drop a file in and it builds
ARCH_SRC := $(wildcard $(ARCHDIR)/*.c)
ARCH_OBJ := $(patsubst kernel/%.c,build/%.o,$(ARCH_SRC))

# the libc is split into one directory per header (stdio/, string/, ...) so this grabs libc/*/*.c
# it stays empty for now which is fine, the linker just gets no extra objects
LIBC_SRC := $(wildcard libc/*/*.c)
LIBC_OBJ := $(patsubst libc/%.c,build/libc/%.o,$(LIBC_SRC))

# every object we compile ourselves, used for the header dependency files further down
OBJS := $(KERN_OBJ) $(ARCH_OBJ) $(LIBC_OBJ)

# flags
# kernel/include holds the kernel only headers, libc/include the ones the libc exposes
# the arch directory is on the include path too so kernel code can pull in vga.h and friends
INCLUDES := -Ikernel/include -Ilibc/include -I$(ARCHDIR)
# -MMD -MP writes a .d file next to every .o so touching a header rebuilds what includes it
CDFLAGS := -ffreestanding -mno-red-zone -m64 -Wall -Wextra -MMD -MP -c $(INCLUDES)
LDFLAGS := -nmagic -T $(LINKER) --oformat binary

# targets
BIN1 := build/stage1.img
OBJ2 := build/stage2.o
BIN2 := build/stage2.img
IMG  := build/disk.img

all: $(IMG)

# compiling stage 1
# -D hands STAGE2_SECTORS to nasm so the dap loads exactly as many sectors as we pad
$(BIN1): $(ST1) Makefile
	@mkdir -p $(@D)
	$(ASM) -f bin -i $(INCDIR) -DSTAGE2_SECTORS=$(STAGE2_SECTORS) $(ST1) -o $(BIN1)

# compiling stage 2 into elf64 object file so we can merge it with the c kernel later
$(OBJ2): $(ST2)
	@mkdir -p $(@D)
	$(ASM) -f elf64 -i $(INCDIR) $(ST2) -o $(OBJ2)

# compile every c file of the kernel into an elf64 object file
# this covers kernel/kernel/ as well as kernel/arch/<arch>/, build/ mirrors the source tree
build/%.o: kernel/%.c
	@mkdir -p $(@D)
	$(CC) $(CDFLAGS) $< -o $@

# same for the libc, its sources sit in subdirectories we mirror into build/ too
build/libc/%.o: libc/%.c
	@mkdir -p $(@D)
	$(CC) $(CDFLAGS) $< -o $@

# linking stage 2, the kernel and the libc together
# stage 2 has to come first since the linker puts it at 0x8000 where stage 1 jumps to
$(BIN2): $(OBJ2) $(OBJS) $(LINKER)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) $(OBJ2) $(OBJS) -o $(BIN2)

# combining both stages into an img file
# first we make sure stage 2 + the kernel still fit in the sectors stage 1 loads
# if they dont we stop right here instead of booting into a triple fault
$(IMG): $(BIN1) $(BIN2) Makefile
	@size=$$(stat -c %s $(BIN2)); \
	if [ $$size -gt $(STAGE2_BYTES) ]; then \
		echo "ERROR: $(BIN2) is $$size bytes but stage 1 only loads $(STAGE2_BYTES)"; \
		echo "       bump STAGE2_SECTORS in the Makefile"; \
		exit 1; \
	fi
	$(CAT) $(BIN1) $(BIN2) > $(IMG)
	$(TRUN) -s $$((512 + $(STAGE2_BYTES))) $(IMG)

# boot the finished image, same thing run.sh does
run: $(IMG)
	qemu-system-x86_64 -drive format=raw,file=$(IMG)

clean:
	rm -rf build

# pull in the header dependencies gcc generated, the dash keeps make quiet on a fresh checkout
-include $(OBJS:.o=.d)

.PHONY: all run clean
