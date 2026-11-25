#include <video/vbe.h>
#include <fonts/fonts.h>

#include <stdint.h>

vbe_mode_info_t VBE;

// global cursor positions
static uint32_t cursor_x;
static uint32_t cursor_y;
// global terminal background color
static uint64_t background_color;
static uint64_t font_color;
// the gap between lines
static uint32_t line_gap;
// scale for text
static uint32_t text_scale;
// max height and width
static uint32_t max_height;
static uint32_t max_width;

const vbe_mode_info_t* load_vbe(const uint64_t vbe_addr) {
    vbe_mode_info_t *vbe = (vbe_mode_info_t*)vbe_addr;

    return vbe;
}

void vbe_terminal_init(uint64_t background, uint64_t font, uint32_t gap) {
    cursor_x = 0;
    cursor_y = 0;
    text_scale = 2;
    background_color = background;
    font_color = font;
    line_gap = gap;

    max_height = VBE.height / (8 * text_scale + line_gap);
    max_width = VBE.width / (8 * text_scale);
}

void vbe_write(char c) {   
    const glyph *character = get_glyph((const char) c);

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {       
        draw_character(character, &VBE, cursor_x * 8 * text_scale, cursor_y * (8  * text_scale + line_gap), font_color, background_color, text_scale);

        cursor_x++;

        if (cursor_x >= max_width) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= max_height) {
        cursor_x = 0;
        cursor_y = max_height - 1;
    }
}

void write_logo_character(char c, uint32_t text_size, uint32_t x, uint32_t y) {
    const glyph *character = get_glyph((const char) c); 

    draw_character(character, &VBE, x, y, font_color, background_color, text_size);
}

void write_logo(char *c) {
    const uint32_t text_size = 10;

    const uint32_t logo_height = 8 * text_size;

    const uint32_t logo_length = 10;
    const uint32_t letter_spacing = 12;
    const uint32_t letter_width = text_size * 8;
    uint32_t x = 20;
    const uint32_t y = 20;

    uint32_t idx = 0;
    while (c[idx] != '\0') {
        write_logo_character(c[idx], text_size, x, y);

        x += letter_width + letter_spacing;

        idx++;
    }

    // since we only render the logo when we boot into the kernel we can reset the y and x here
    cursor_x = 0;
    cursor_y = text_size/2;    
}

void clear() {
    fill_rect(&VBE, 0, 0, VBE.width, VBE.height, 0xFFFFFF);
}