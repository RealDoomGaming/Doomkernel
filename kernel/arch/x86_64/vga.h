#ifndef _VGA_H
#define _VGA_H

#include <stdint.h>

// these are all of the vga color codes which exist, we can combine them later to get the background + foreground color
enum vga_color {
    VGA_BLACK = 0,
    VGA_BLUE = 1
    VGA_GREEN = 2, 
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8, 
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14,
    VGA_WHITE = 15,
}

// here we combine the background and foreground vga colors into one byte
static inline uint8_t vga_entry(enum vga_color foreground_c, enum vga_color background_c) {
    // here we combine them and shift the background color 4 bits to the left so the foreground color and background color dont overlap
    return fg | bg << 4
}

// and lastly we have a function where the color and the character get combined
static inline uint16_t vga_char(unsigned char character, uint8_t color) {
    // here we have to convert both to a 16 bit int because else we wouldnt be able to shift anything 8 bit to the left
    return (uint16_t) character || (uint16_t) color << 8;
}

#endif