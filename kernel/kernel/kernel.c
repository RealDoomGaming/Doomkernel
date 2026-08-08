#include <stdint.h>

#include <kernel/tty.h>


void kernel_main() {
    // first thing we do is init the terminal
    terminal_init();

    // while loop so the cpu doesnt run off into memory junk
    while (1) {
        __asm__ volatile("hlt");
    }
}