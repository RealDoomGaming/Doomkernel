#ifndef __VBE_H
#define __VBE_H

#include <stdint.h>
#include <fonts/fonts.h>

typedef struct __attribute__((packed)) {
    uint16_t attributes;        // 0x00 bit 7 = linear framebuffer supported
    uint8_t  window_a;          // 0x02 old banking stuff, we dont use it
    uint8_t  window_b;          // 0x03
    uint16_t granularity;       // 0x04
    uint16_t window_size;       // 0x06
    uint16_t segment_a;         // 0x08
    uint16_t segment_b;         // 0x0A
    uint32_t win_func_ptr;      // 0x0C
    uint16_t pitch;             // 0x10 bytes per row, NOT always width * bytes per pixel
    uint16_t width;             // 0x12 pixels
    uint16_t height;            // 0x14 pixels
    uint8_t  w_char;            // 0x16
    uint8_t  y_char;            // 0x17
    uint8_t  planes;            // 0x18
    uint8_t  bpp;               // 0x19 bits per pixel
    uint8_t  banks;             // 0x1A
    uint8_t  memory_model;      // 0x1B 6 = direct color
    uint8_t  bank_size;         // 0x1C
    uint8_t  image_pages;       // 0x1D
    uint8_t  reserved0;         // 0x1E
    uint8_t  red_mask_size;     // 0x1F
    uint8_t  red_position;      // 0x20
    uint8_t  green_mask_size;   // 0x21
    uint8_t  green_position;    // 0x22
    uint8_t  blue_mask_size;    // 0x23
    uint8_t  blue_position;     // 0x24
    uint8_t  reserved_mask_size;// 0x25
    uint8_t  reserved_position; // 0x26
    uint8_t  direct_color_info; // 0x27
    uint32_t framebuffer;       // 0x28 PHYSICAL address of the framebuffer
} vbe_mode_info_t;

// this is for the vbe in the kernel
extern vbe_mode_info_t VBE;

const vbe_mode_info_t *load_vbe(const uint64_t);

// framebuffer stuff
uint16_t compute_pdpt(uint32_t);
uint16_t compute_dir(uint32_t);
void fill_dir_entries(uint16_t, uint16_t, vbe_mode_info_t*);

// stuff to draw pixels
void draw_character(const glyph*, const vbe_mode_info_t*, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
void fill_rect(const vbe_mode_info_t *, uint32_t, uint32_t,
               uint32_t, uint32_t, uint32_t);
void clear();

void vbe_terminal_init(uint64_t, uint64_t, uint32_t);
void vbe_write(char);
void write_logo(char*);

#endif