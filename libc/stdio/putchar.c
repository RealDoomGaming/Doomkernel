#include <stdio.h>
#include <kernel/tty.h>
#include <com-ports/com1.h>

// this is for writing one character to the terminal
int putchar(int int_char) {
    char c = (char) int_char;
    // this just calls the function from the tty.h file which outputs to the vga
    terminal_write(&c, sizeof(c));
    // we also want to write it to the com1 so it also gets outputted if we are in vbe mode
    write_string(&c);

    return int_char;
}