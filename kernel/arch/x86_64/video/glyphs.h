#ifndef __GLYPHS_H
#define __GLYPHS_H

#include <stdint.h>

typedef struct {
    uint8_t data[8];    // this is the data how the glyph will be mapped
} glyph;

#endif