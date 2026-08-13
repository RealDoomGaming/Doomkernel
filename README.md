# Doomkernel
Doomkernel is my own 64 bit kernel written from scratch which boots using my own bootloader: [Doomboot](https://github.com/RealDoomGaming/Doomboot)

## Roadmap
### Phase 0: Foundations
- [ ] Set up cross-compiler toolchain (`x86_64-elf-gcc`, `binutils`) -> maybe?
- [x] Custom bootloader (Doomboot)
- [x] Boot into 64-bit long mode kernel

### Phase 1: Core Kernel
- [x] Create a Hello World kernel
- [x] Set up a project / build infrastructure
- [ ] Call global constructors
- [x] Terminal support (formatted printf-style printing)
- [ ] Stack smashing protector
- [ ] Multiboot support
- [ ] Global Descriptor Table (GDT)
- [ ] Memory management (physical/virtual, heap)
- [ ] Interrupts
- [ ] Multithreaded kernel
- [ ] Keyboard support
- [ ] Internal kernel debugger
- [ ] Filesystem support (+ initialization ramdisk)

### Phase 2: User-Space
- [ ] User-space / switch to user mode
- [ ] Program loading (dynamic linker)
- [ ] System calls
- [ ] OS-specific toolchain
- [ ] Creating a C library
- [ ] Fork and execute
- [ ] Shell

## Build & Run
### Requirements:
| Tool | What it's for |
|------|----------------|
| `x86_64-elf-gcc` | Cross-compiler that builds freestanding C for the kernel |
| `nasm` | Assembles any low-level `.asm` stubs (entry point, ISRs, etc.) |
| `qemu-system-x86_64` | Emulates a PC to actually boot the kernel |
| `make` | Runs the build |
 
### Run:
```bash
make clean
chmod +x ./run.sh
./run.sh
```
