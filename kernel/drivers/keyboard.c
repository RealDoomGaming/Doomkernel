#include <stdint.h>

#include <include/keyboard/keyboard.h>

// here we define a buffer for the keycache
#define KBD_BUFFER_SIZE 256

// these are some variables we will need everywhere later on so we define them here
static uint8_t keycache[KBD_BUFFER_SIZE];
static uint16_t kbd_head;
static uint16_t kbd_tail;
static uint8_t __kbd_enabled;

// then we make a table for the ascii lookup table
static const char scancode_ascii[] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
   '*',   0, ' '
};

