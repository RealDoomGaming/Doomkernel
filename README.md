# Doomkernel
Doomkernel is my own 64 bit kernel written from scratch which boots using my own bootloader: [Doomboot](https://github.com/RealDoomGaming/Doomboot)

## Roadmap
- [ ] I dont know what to put here just yet

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
