#include <kernel/tty.h>

#include "vga.h"

// we need to use the uint16_t* type here since for vga every character is stored as 16bit and not like normally as 8bit
#define VGA ((uint16_t*) 0xB8000)
// width and height of the screen in characters
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// we need a global cursor position state
static int cursor_x;
static int cursor_y;
// we need a global terminal color
static uint8_t terminal_color;
// we also need a global buffer so we dont work wit hthe memory of the vga directly
static uint16_t* terminal_buffer;

// function later for clearing the screen
void clear_screen();

// the actual function for initing the terminal
void terminal_init() {
    // when we init the terminal we want to set the cursors to 0,0
    // then we want to set the terminal color to be white color on black background
    // and also want the terminal buffer to point to the vga memory 
    // and then the most important thing, we want to clear it
    cursor_x = 0;
    cursor_y = 0;
    terminal_color = vga_entry(VGA_WHITE, VGA_BLACK);
    terminal_buffer = VGA;
    clear_screen();
}

void clear_screen() {
    uint16_t blank = vga_char(' ', terminal_color);

    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        terminal_buffer[i] = blank;
    }
}

void terminal_put_char(char c) {
    // when we want to print a char we firstly need to check if its a newline or a carriage return
    // and only then can we combine the character and the color into a 16 bit entry and put it inot the VGA buffer

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {
        int index = (cursor_y * VGA_WIDTH) + cursor_x;

        terminal_buffer[index] = vga_char(c, terminal_color);

        cursor_x++;
        if (cursor_x >= VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= VGA_HEIGHT) {
        clear_screen();
        cursor_y = 0;
    }
}

void terminal_write(const char* data, size_t length) {
    for (size_t i = 0; i < length; i++) {
        terminal_put_char(data[i]);
    } 
}