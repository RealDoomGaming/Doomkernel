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
STAGE2_SECTORS := 64
STAGE2_BYTES := $(shell expr $(STAGE2_SECTORS) \* 512)

# same idea as STAGE2_SECTORS: a reserved budget with headroom. the build
# fails loudly if the real initrd ever outgrows it instead of silently
# loading a truncated filesystem
INITRD_SECTORS := 64
INITRD_BYTES := $(shell expr $(INITRD_SECTORS) \* 512)

# host tool that builds the initrd - uses your SYSTEM gcc, not the cross
# compiler, since it runs on your machine, not the target
HOSTCC := gcc

# the bootloader lives in its own directory so it stays a self contained project we can update on its own
BOOTDIR := Doomboot
ST1  := $(BOOTDIR)/src/stage1.asm
ST2  := $(BOOTDIR)/src/stage2.asm
LINKER := $(BOOTDIR)/linker/linker.ld
INCDIR := $(BOOTDIR)/src/

# every c file one level under kernel/ is part of the kernel, that covers kernel/kernel/, kernel/interrupts/,
# kernel/drivers/ and any directory we add later so dropping a new file in needs no change down here
KERN_SRC := $(wildcard kernel/*/*.c)
KERN_OBJ := $(patsubst kernel/%.c,build/%.o,$(KERN_SRC))

# the architecture specific part of the kernel (tty, ports, gdt, ...) sits one level deeper
# so it gets its own wildcard, only the arch we build for is picked up
ARCH_SRC := $(wildcard $(ARCHDIR)/*.c)
ARCH_OBJ := $(patsubst kernel/%.c,build/%.o,$(ARCH_SRC))

# the asm parts of the kernel like the isr/irq stubs, assembled to elf64 so they link with the c code
# same split as above, one wildcard for the generic parts and one for the arch specific ones
KASM_SRC := $(wildcard kernel/*/*.asm) $(wildcard $(ARCHDIR)/*.asm)
KASM_OBJ := $(patsubst kernel/%.asm,build/%.o,$(KASM_SRC))

# the libc is mostly split into one directory per header (stdio/, string/, ...) but files like panic.c
# sit directly in libc/ so we grab both levels
LIBC_SRC := $(wildcard libc/*.c) $(wildcard libc/*/*.c)
LIBC_OBJ := $(patsubst libc/%.c,build/libc/%.o,$(LIBC_SRC))

# every c object we compile ourselves, used for the header dependency files further down
# the asm objects stay out of here since nasm writes no .d files
OBJS := $(KERN_OBJ) $(ARCH_OBJ) $(LIBC_OBJ)

# flags
# kernel/include holds the kernel only headers, libc/include the ones the libc exposes
# the arch directory is on the include path too so kernel code can pull in vga.h and friends
INCLUDES := -Ikernel/include -Ilibc/include -I$(ARCHDIR)
# -MMD -MP writes a .d file next to every .o so touching a header rebuilds what includes it
CDFLAGS := -ffreestanding -mno-red-zone -mgeneral-regs-only -m64 -Wall -Wextra -MMD -MP -c $(INCLUDES)
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
	$(ASM) -f elf64 -i $(INCDIR) -DSTAGE2_SECTORS=$(STAGE2_SECTORS) -DINITRD_SECTORS=$(INITRD_SECTORS) $(ST2) -o $(OBJ2)

build/tools/pack_initrd: tools/pack_initrd.c
	@mkdir -p $(@D)
	$(HOSTCC) -Wall -Wextra $< -o $@

INITRD_FILES := $(wildcard initrd/*)
INITRD_BIN := build/initrd.bin

$(INITRD_BIN): build/tools/pack_initrd $(INITRD_FILES)
	@mkdir -p $(@D)
	build/tools/pack_initrd $(INITRD_BIN) $(INITRD_FILES)
	@size=$$(stat -c %s $(INITRD_BIN)); \
	if [ $$size -gt $(INITRD_BYTES) ]; then \
		echo "ERROR: $(INITRD_BIN) is $$size bytes but only $(INITRD_BYTES) bytes are reserved"; \
		echo "       bump INITRD_SECTORS in the Makefile"; \
		exit 1; \
	fi

# compile every c file of the kernel into an elf64 object file
# this covers kernel/kernel/, kernel/interrupts/ as well as kernel/arch/<arch>/, build/ mirrors the source tree
build/%.o: kernel/%.c
	@mkdir -p $(@D)
	$(CC) $(CDFLAGS) $< -o $@

# the asm files that belong to the kernel itself, elf64 like stage 2 so the linker can merge them
build/%.o: kernel/%.asm
	@mkdir -p $(@D)
	$(ASM) -f elf64 $< -o $@

# same for the libc, its sources sit in subdirectories we mirror into build/ too
build/libc/%.o: libc/%.c
	@mkdir -p $(@D)
	$(CC) $(CDFLAGS) $< -o $@

# linking stage 2, the kernel and the libc together
# stage 2 has to come first since the linker puts it at 0x8000 where stage 1 jumps to
$(BIN2): $(OBJ2) $(OBJS) $(KASM_OBJ) $(LINKER)
	@mkdir -p $(@D)
	$(LD) $(LDFLAGS) $(OBJ2) $(OBJS) $(KASM_OBJ) -o $(BIN2)

# combining both stages into an img file
# first we make sure stage 2 + the kernel still fit in the sectors stage 1 loads
# if they dont we stop right here instead of booting into a triple fault
$(IMG): $(BIN1) $(BIN2) $(INITRD_BIN) Makefile
	@size=$$(stat -c %s $(BIN2)); \
	if [ $$size -gt $(STAGE2_BYTES) ]; then \
		echo "ERROR: $(BIN2) is $$size bytes but stage 1 only loads $(STAGE2_BYTES)"; \
		echo "       bump STAGE2_SECTORS in the Makefile"; \
		exit 1; \
	fi
	$(CAT) $(BIN1) $(BIN2) > $(IMG)
	$(TRUN) -s $$((512 + $(STAGE2_BYTES))) $(IMG)
	$(CAT) $(INITRD_BIN) >> $(IMG)
	$(TRUN) -s $$((512 + $(STAGE2_BYTES) + $(INITRD_BYTES))) $(IMG)

# boot the finished image, same thing run.sh does
run: $(IMG)
	qemu-system-x86_64 -drive format=raw,file=$(IMG)

clean:
	rm -rf build

# pull in the header dependencies gcc generated, the dash keeps make quiet on a fresh checkout
-include $(OBJS:.o=.d)

.PHONY: all run clean
