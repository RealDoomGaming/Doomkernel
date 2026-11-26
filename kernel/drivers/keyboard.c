#include <stdint.h>
#include <stdio.h>
#include <interrupts/isr.h>
#include <keyboard/keyboard.h>
#include <io/ports.h>
#include <interrupts/pic.h>
#include <video/vbe.h>
#include "scancodes.h"

// here we define a buffer for the keycache
#define KBD_BUFFER_SIZE 256

// these are some variables we will need everywhere later on so we define them here
static volatile uint8_t keycache[KBD_BUFFER_SIZE];
static volatile uint16_t kbd_head;
static volatile uint16_t kbd_tail;
static uint8_t kbd_shift;
static uint8_t kbd_caps;
static uint8_t kbd_extended;


static char keycode_to_char(uint8_t scancode) {
  return kbd_shift ? scancode_map[scancode].shifted : scancode_map[scancode].normal;
}

static void kbd_buffer_push(uint8_t key) {
  // we firstly calculate the next index in the buffer
  uint16_t next = (kbd_head + 1) & (KBD_BUFFER_SIZE - 1);

  // then we check if the array is already full
  if (next == kbd_tail) {
    return;
  }

  // then if we know the array isnt full we can store the key
  keycache[kbd_head] = key;
  kbd_head = next;
}

void keyboard_handler(interrupt_frame_t *__frame) {
  // this is the scancode of the keyboard input
  uint8_t scancode = inb(0x60);

  // if the scancode is 0xE0 then we know that the next scancode belongs to an extended key so we do nothing with it
  if (scancode == 0xE0) {
    // and we also set kbd
    kbd_extended = 1;
    return;
  }

  // void scroll(uint8_t up_down)
  // if arrow key down was pressed we scroll down
  if (scancode == 0x50) {
    scroll(0);
    return;
  }

  // if arrow key up was pressed we scroll up
  if (scancode == 0x48) {
    scroll(1);
    return;
  }

  // we get the top bit here, this controls whatever the next key is treated as extended or not
  uint8_t top_bit = scancode & 0x80;
  // and the bottom 7 bit here, this say which key it actually is (we dont need it but imma just have it stand here)
  // uint8_t bottom_bit = scancode & 0x7F;

  if (scancode == 0x2A || scancode == 0x36) {
    // if we press left or rigt shift then we set the shift to on
    kbd_shift = 1;
    return;
  } else if (scancode == 0xAA || scancode == 0xB6) {
    // if we realease left or right shift then we set it to 0
    kbd_shift = 0;
    return;
  } else if (scancode == 0x3A) {
    // for toggeling caps lock
    kbd_caps = !kbd_caps;
    return;
  }

  // if its any other kind of release then the shift then we just return
  if (top_bit) {
    return;
  }

  // if the scancode we got is bigger than the table we have we just return
  if (scancode > SCANCODE_COUNT) {
    return;
  }

  char key = keycode_to_char(scancode);

  // if what we got is 0 then that means that we dont have a character to print
  if (key == 0) {
    return;
  }

  // then if caps lock is set we basically just invert everything
  if (kbd_caps) {
    // we do + and - 32 to reach the other character in hex
    if (key >= 'a' && key <= 'z') {
      key -= 32;
    } else {
      key += 32;
    }
  }
  
  kbd_buffer_push((uint8_t) key);
}

void keyboard_init() {
  // firstly we reset all our global variables
  kbd_head = 0;
  kbd_tail = 0;
  kbd_caps = 0;
  kbd_shift = 0;
  kbd_extended = 0;

  // then we register the interrupt to 33 (0x20)
  register_interrupt_handler(33, keyboard_handler);

  // then we have to unmask
  pic_clear_mask(1);
}

char keyboard_get_key() {
  // this function basically just waits until we get a key and then returns it

  // firstly we have to wait for key
  while (kbd_head == kbd_tail) {
    __asm__ volatile ("hlt");
  }


  // we get our character form the buffer
  char character = (char)keycache[kbd_tail];
  // then we have to advance the kbd tail by one
  kbd_tail = (kbd_tail + 1) & (KBD_BUFFER_SIZE - 1);

  // then lastly we can return our character
  return character;
}

int keyboard_has_key() {
  // this function just checks if kbd_head and kbd_tail differ from eachother
  return kbd_head - kbd_tail;
}