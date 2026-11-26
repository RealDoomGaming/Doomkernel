#include <video/vbe.h>
#include <fonts/fonts.h>
#include <keyboard/keyboard.h>
#include <timer/pit.h>

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define BUF_ROWS 500
#define BUF_COLS 64

vbe_mode_info_t VBE;


// we define an array which saves all of the stuff printed so we can build in scroll later
static char characters[BUF_ROWS][BUF_COLS];
// a variable which tells at which line in the buffer we currently are
static uint64_t current_line;
// a variable which defines which line we are at in our buffer
static uint64_t head;
// a variable which defines what line gets shown last on top of the screen
static uint64_t tail;
// then a variable which will help us when redrawing the screen
static uint64_t V;

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
// the two variables for the cursor status
static uint8_t cursor_shown;
static uint64_t blink_start;

void reset_buffer();
void new_line();

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
    cursor_shown = 0;
    blink_start = 0;
    head = 0;
    tail = 0;

    max_height = VBE.height / (8 * text_scale + line_gap);
    max_width = VBE.width / (8 * text_scale);

    reset_buffer();
}

void write_cursor(char c) {
    const glyph *character = get_glyph((const char) c);
    draw_character(character, &VBE, cursor_x * 8 * text_scale, cursor_y * (8  * text_scale + line_gap), font_color, background_color, text_scale);
}

void show_cursor() {
    write_cursor('_');
    cursor_shown = 1;
}

void hide_cursor() {
    write_cursor(' ');
    cursor_shown = 0;
}

void clear_row(uint64_t line) {
    for (uint32_t col = 0; col < BUF_COLS; col++) {
        characters[line][col] = ' ';
    }
}

void shift_up() {
    for (uint64_t i = 0; i < head; i++) {
        // in this loop we copy the next row into this row 
        // memcpy(void* destptr, const void* srcptr, size_t size)
        memcpy(characters[i], characters[i+1], sizeof(char) * BUF_COLS);
    }
}

void redraw_screen() {
    uint64_t line = 0;
    uint64_t screen_row = 0;
    uint64_t y = 0;
    uint64_t x = 0;
    char c;
    V = max_height - tail;

    uint64_t row = 0;
    uint64_t col = 0;
    // either we stop when we looped over all lines or when we are out of lines
    while (row < V && row <= head) {
        line = head - row;
        screen_row = tail + (V - 1 - row);
        y = screen_row * (8 * text_scale + line_gap);

        while (col < max_width) {
            c = characters[line][col];
            const glyph *character = get_glyph(c);

            x = col * 8 * text_scale;

            draw_character(character, &VBE, x, y, font_color, background_color, text_scale);

            col++;
        }

        col = 0;
        row++;
    }

}

void new_line() {
    // in this function if the head ocunter isnt as as big as the amount of lines max then increase it
    if (head < BUF_ROWS - 1) {
        head++;
        current_line++;
    } else {
        shift_up();
        clear_row(head);
    }
}

void store_character(char c) {
    characters[head][cursor_x] = c;
}

void scroll(uint8_t up_down) {
    // if up_down is 1 then we want to scroll up else if its 0 we want to scroll down
    if (up_down == 1) {
        if (tail > 0) {
            tail--;
            head--;
        }
    } else if (up_down == 0){
        if (head <= current_line) {
            tail++;
            head++;
        }
    }

    redraw_screen();
}

void vbe_write(char c) {   
    const glyph *character = get_glyph((const char) c);
    hide_cursor();

    if (c == '\n') {
        new_line();
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {       
        draw_character(character, &VBE, cursor_x * 8 * text_scale, cursor_y * (8  * text_scale + line_gap), font_color, background_color, text_scale);
        store_character(c);

        cursor_x++;

        if (cursor_x >= max_width) {
            new_line();
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y >= max_height) {
        cursor_x = 0;
        cursor_y = max_height - 1;
        redraw_screen();
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

void reset_buffer() {
    for (uint64_t line = 0; line < BUF_ROWS; line++) {
        clear_row(line);
    }
    head = 0;
}

void clear() {
    cursor_x = 0;
    cursor_y = 0;
    reset_buffer();
    fill_rect(&VBE, 0, 0, VBE.width, VBE.height, background_color);
}

char wait_for_key() {
    uint32_t phase;

    while (!keyboard_has_key()) {
        phase = ((ticks - blink_start) / 50) % 2;

        if (phase == 0) {
            show_cursor();
        } else if (phase == 1) {
            hide_cursor();
        }

        __asm__ volatile ("hlt");
    }

    hide_cursor();
    blink_start = ticks;
    return keyboard_get_key();
}