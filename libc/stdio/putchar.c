#include <stdio.h>
#include <kernel/tty.h>

// this is for writing one character to the terminal
int putchar(int int_char) {
    char c = (char) int_char;
    // this just calls the function from the tty.h file which outputs to the vga
    terminal_write(&c, sizeof(c));

    return int_char;
}