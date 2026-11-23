#include <fonts/fonts.h>

const glyph *get_glyph(const char c) {
    // this function is for giving back the glyph which fits to the character

    // the character we need to search for
    unsigned char c_u = (unsigned char)c;

    // if the character is not in our table we dont do anything
    if (c_u >= 128) {
        c_u = ' ';
    }

    // we need to return a pointer so we add & to the front
    return &glyph_table[c_u];
}