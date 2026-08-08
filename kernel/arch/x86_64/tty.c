#include <kernel/tty.h>

#include "vga.h"

// we need to use the uint16_t* type here since for vga every character is stored as 16bit and not like normally as 8bit
#define VGA (uint16_t*) 0xB8000
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

void terminal_write()