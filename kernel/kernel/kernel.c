#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <kernel/tty.h>

extern uint64_t kernel_end;

void kernel_main() {
    // first thing we do is init the terminal
    terminal_init();

    // here we init the entire interrupt stuff
    idt_init();
    pic_remap(0x20, 0x28);
    __asm__ volatile("sti");

    // and then we test it 
    __asm__ volatile("int3"); // this would give us a panic

    // then we init the memory
    memory_init((uint64_t)&kernel_end);

    // printing with our custom printf function :DD
    printf("Successfully booted into the kernel!\n");

    // while loop so the cpu doesnt run off into memory junk
    while (1) {
        __asm__ volatile("hlt");
    }
}