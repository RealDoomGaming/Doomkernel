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
static uint8_t background_color;
static uint8_t font_color;

const vbe_mode_info_t* load_vbe(const uint64_t vbe_addr) {
    vbe_mode_info_t *vbe = (vbe_mode_info_t*)vbe_addr;

    return vbe;
}

void vbe_terminal_init(uint8_t background, uint8_t font) {
    cursor_x = 0;
    cursor_y = 0;
    background_color = background;
    font_color = font;
}

void vbe_write(char c) {   
    const glyph *character = get_glyph((const char) c);

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x == 0;
    } else {
        int index = (cursor_y * MAX_WIDTH) + cursor_x;
        
        draw_character(character, VBE, cursor_x, cursor_y, font_color, background_color);
 

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