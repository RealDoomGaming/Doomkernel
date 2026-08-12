#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <kernel/tty.h>


void kernel_main() {
    // first thing we do is init the terminal
    terminal_init();

    // then we init the memory
    memory_init();

    // printing with our custom printf function :DD
    printf("Successfully booted into the kernel!\n");

    // while loop so the cpu doesnt run off into memory junk
    while (1) {
        __asm__ volatile("hlt");
    }
}