#include <video/vbe.h>
#include <fonts/fonts.h>

#include <stdint.h>

vbe_mode_info_t VBE;

#define MAX_HEIGHT 95
#define MAX_WIDTH 127

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

const vbe_mode_info_t* load_vbe(const uint64_t vbe_addr) {
    vbe_mode_info_t *vbe = (vbe_mode_info_t*)vbe_addr;

    return vbe;
}

void vbe_terminal_init(uint64_t background, uint64_t font, uint32_t gap) {
    cursor_x = 0;
    cursor_y = 0;
    text_scale = 1;
    background_color = background;
    font_color = font;
    line_gap = gap;
}

void vbe_write(char c) {   
    const glyph *character = get_glyph((const char) c);

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {       
        draw_character(character, &VBE, cursor_x * 8, cursor_y * (8 + line_gap), font_color, background_color, text_scale);

        cursor_x++;

        if (cursor_x >= MAX_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= MAX_HEIGHT) {
        cursor_x = 0;
        cursor_y = MAX_HEIGHT - 1;
    }
}