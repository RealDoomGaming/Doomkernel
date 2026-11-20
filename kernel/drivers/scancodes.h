#ifndef __SCANCODES_H
#define __SCANCODES_H

#include <dictionary.h>

#define SCANCODE_COUNT 0x3A

static const key_entry_t scancode_map[SCANCODE_COUNT] = {
    [0x00] = { 0,    0    },   // unused
    [0x01] = { 27,   27   },   // escape
    [0x02] = { '1',  '!'  },   // 1
    [0x03] = { '2',  '@'  },   // 2
    [0x04] = { '3',  '#'  },   // 3
    [0x05] = { '4',  '$'  },   // 4
    [0x06] = { '5',  '%'  },   // 5
    [0x07] = { '6',  '^'  },   // 6
    [0x08] = { '7',  '&'  },   // 7
    [0x09] = { '8',  '*'  },   // 8
    [0x0A] = { '9',  '('  },   // 9
    [0x0B] = { '0',  ')'  },   // 0
    [0x0C] = { '-',  '_'  },   // minus
    [0x0D] = { '=',  '+'  },   // equals
    [0x0E] = { '\b', '\b' },   // backspace
    [0x0F] = { '\t', '\t' },   // tab
    [0x10] = { 'q',  'Q'  },   // Q
    [0x11] = { 'w',  'W'  },   // W
    [0x12] = { 'e',  'E'  },   // E
    [0x13] = { 'r',  'R'  },   // R
    [0x14] = { 't',  'T'  },   // T
    [0x15] = { 'z',  'Z'  },   // Z
    [0x16] = { 'u',  'U'  },   // U
    [0x17] = { 'i',  'I'  },   // I
    [0x18] = { 'o',  'O'  },   // O
    [0x19] = { 'p',  'P'  },   // P
    [0x1A] = { '[',  '{'  },   // open bracket
    [0x1B] = { ']',  '}'  },   // close bracket
    [0x1C] = { '\n', '\n' },   // enter
    [0x1D] = { 0,    0    },   // left ctrl
    [0x1E] = { 'a',  'A'  },   // A
    [0x1F] = { 's',  'S'  },   // S
    [0x20] = { 'd',  'D'  },   // D
    [0x21] = { 'f',  'F'  },   // F
    [0x22] = { 'g',  'G'  },   // G
    [0x23] = { 'h',  'H'  },   // H
    [0x24] = { 'j',  'J'  },   // J
    [0x25] = { 'k',  'K'  },   // K
    [0x26] = { 'l',  'L'  },   // L
    [0x27] = { ';',  ':'  },   // semicolon
    [0x28] = { '\'', '"'  },   // apostrophe
    [0x29] = { '`',  '~'  },   // backtick
    [0x2A] = { 0,    0    },   // left shift
    [0x2B] = { '\\', '|'  },   // backslash
    [0x2C] = { 'y',  'Y'  },   // Y
    [0x2D] = { 'x',  'X'  },   // X
    [0x2E] = { 'c',  'C'  },   // C
    [0x2F] = { 'v',  'V'  },   // V
    [0x30] = { 'b',  'B'  },   // B
    [0x31] = { 'n',  'N'  },   // N
    [0x32] = { 'm',  'M'  },   // M
    [0x33] = { ',',  '<'  },   // comma
    [0x34] = { '.',  '>'  },   // period
    [0x35] = { '/',  '?'  },   // slash
    [0x36] = { 0,    0    },   // right shift
    [0x37] = { '*',  '*'  },   // keypad multiply
    [0x38] = { 0,    0    },   // left alt
    [0x39] = { ' ',  ' '  },   // space
};

#endif