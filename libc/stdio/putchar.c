#include <stdio.h>
#include <globals.h>
#include <kernel/tty.h>
#include <video/vbe.h>
#include <com-ports/com1.h>

// this is for writing one character to the terminal
int putchar(int int_char) {
    char c = (char) int_char;
    // this just calls the function from the tty.h file which outputs to the vga

    // if we dont have a vbe mode we do this
    if (DISPLAY_STATE == output_state.VGA) {
        terminal_write(&c, sizeof(c));
    } else if (DISPLAY_STATE == output_state.VBE) {
        // else we do this
        vbe_write(c);
    }

    // we also want to write it to the com1 so it also gets outputted if we are in vbe mode
    // for testing we wrote this: write_com1((uint8_t)int_char);



    return int_char;
}