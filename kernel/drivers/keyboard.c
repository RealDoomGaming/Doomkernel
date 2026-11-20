#include <stdint.h>
#include <interrupts/isr.h>
#include <keyboard/keyboard.h>
#include <io/ports.h>
#include <interrupts/pic.h>

// here we define a buffer for the keycache
#define KBD_BUFFER_SIZE 256

// these are some variables we will need everywhere later on so we define them here
volatile static uint8_t keycache[KBD_BUFFER_SIZE];
volatile static uint16_t kbd_head;
volatile static uint16_t kbd_tail;
static uint8_t kbd_shift;
static uint8_t kbd_caps;
static uint8_t kbd_extended;
static uint8_t __kbd_enabled;

// then we make a table for the ascii lookup table
static const char scancode_ascii[] = {
    0,   27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'z', 'u', 'i', 'o', 'p', '[', ']', '\n',
     0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
     0, '\\', 'y', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
   '*',   0, ' '
};
// same table but this one will be used when shift is pressed
static const char scancode_ascii_shift[] = {
    0,   27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
  '\t', 'Q', 'W', 'E', 'R', 'T', 'Z', 'U', 'I', 'O', 'P', '{', '}', '\n',
     0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
     0, '|', 'Y', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
   '*',   0, ' '
};
/*
void kbd_buffer_push(char character) {
  // this function is for pushing character we get from when a key is pressed to our buffer
  // firstly we push the character to wherever kbd_head is pointing
  keycache[kbd_head] = character;

  // then we advance the kbd_head with bitwise wrapping so if kbd_head where to advance outside the buffer it goes back to its start
  // if the buffer size is a power of 2 we can use the bitwise AND to make kbd_head 0 again if it becomes too big
  kbd_head = (kbd_head + 1) & (KBD_BUFFER_SIZE - 1);

  if (kbd_head == kbd_tail) {
    // then if kbd_head collides with the tail we move the tail forwards by 1 to discard the oldest character
    kbd_tail = (kbd_tail + 1) & (KBD_BUFFER_SIZE - 1);
  }
}

void keyboard_handler(interrupt_frame_t *frame) {
  (void)frame;

  // we read one byte from the 0x60 port which is the data port in our case
  // we also tell the controller that we have processed the key interrupt
  uint8_t scancode = inb(0x60);

  // we check if the interrupt we got was that a key was released by making their most significant bit be set 
  if (scancode & 0x80) {
    // then we revert it here since we changed the value of scancode in the if
    uint8_t released = scancode & ~0x80;

    // then we know that a key was released but we only care if a key was released if it was the shift key so we check if we got the right or left shift
    if (released == 0x2A || released == 0x36) {
      kbd_shift = 0;
    }

    return;
  }

  // then if we know a key was pressed we check if it was the shift key
  if (scancode == 0x2A || scancode == 0x36) {
    kbd_shift = 1;
    return;
  }

  // then we check if caps lock was pressed
  if (scancode == 0x3A) {
    // and here we use the ^ since this switches the capslock between 0 and 1 every time it gets pressed
    kbd_caps ^= 1;
    return;
  }

  // if the key which was pressed cannot be handled we just return
  if (scancode >= sizeof(scancode_ascii)) {
    return;
  }

  // then after handeling everything else we have to see if the key was pressed while shift was held or not
  char key = kbd_shift ? scancode_ascii_shift[scancode] : scancode_ascii[scancode];

  // and then if we failed to get the key from our table return
  if (key == 0) {
    return;
  }

  // then we just need to check if caps lock is pressed
  if (kbd_caps) {
    // then we need to handle lower case characters gettting converted to upper case characters and the same for upper case ones
    if (key >= 'a' && key <= 'z') {
      key = (char)(key - 32);
    } else if (key >= 'A' && key <= 'Z') {
      key = (char)(key + 32);
    }
  }

  // lastly we have to push the key character whatever you wanna call it to the table
  kbd_buffer_push(key);
}

void keyboard_init() {
    // in this function we init, you guessed it, the keyboard
    // firstly we set the head and tail of the keyboard cache
    kbd_head = 0;
    kbd_tail = 0;
    kbd_caps = 0;
    kbd_shift = 0;

    // we register the keyboard handler
    register_interrupt_handler(33, keyboard_handler);
    // we also need to clear the pic mask
    pic_clear_mask(1);

    // then we set the keyboard enabled to 1 so true
    __kbd_enabled = 1;
}

char keyboard_get_key() {
  // this function is for getting a key when it gets typed
  // and the first thing we do is wait until a key gets typed
  while (kbd_head == kbd_tail) {
    __asm__ volatile("hlt");
  }

  // then we actually get the character from our buffer 
  char character = (char)keycache[kbd_tail];
  // then we have to advance the tail by 1
  kbd_tail = (kbd_tail + 1) & (KBD_BUFFER_SIZE - 1);
  // and lastly return the character we have read
  return character;
}

int keyboard_has_key() {
  // we also want a function for checking if the buffer already has a key
  return kbd_tail != kbd_head;
}*/

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


void keyboard_init() {

}

void keyboard_handler(interrupt_frame_t *frame) {
  // this is the scancode of the keyboard input
  uint8_t scancode = inb(0x60);

  // if the scancode is 0xE0 then we know that the next scancode belongs to an extended key so we do nothing with it
  if (scancode = 0xE0) {
    // and we also set kbd
    kbd_extended = 1;
    return;
  }

  // we get the top bit here, this controls whatever the next key is treated as extended or not
  uint8_t top_bit = scancode & 0x80;
  // and the bottom 7 bit here, this say which key it actually is (we dont need it but imma just have it stand here)
  // uint8_t bottom_bit = scancode & 0x7F;

  if (scancode == 2A || scancode == 36) {
    // if the most significant bit was set then we know we let a key go
    if (top_bit) {
      // and we can set shift to false
      kbd_shift = 0;
      return;
    }

    // if we press left or rigt shift then we set the shift to on
    kbd_shift = 1
    return;
  }

  if (scancode == 3A) {
    // if the caps lock got pressed we automatically put it to whatever it isnt so we can turn it of and on
    kbd_caps = !kbd_caps;
    return;
  }

  // if its any other kind of release then the shift then we just return
  if (top_bit) {
    return;
  }

  // if the scancode we got is bigger than the table we have we just return
  if (scancode > sizeof(scancode_ascii)) {
    return;
  }

  char key = kbd_shift ? scancode_ascii_shift[scancode] : scancode_ascii[scancode];

  // if what we got is 0 then that means that we dont have a character to print
  if (key == 0) {
    return;
  }

  // then if caps lock is set we basically just invert everything
  if (kbd_caps) {
    if (key >= 'a' && key <= 'z') {
      key = scancode_ascii_shift[scancode];
    } else {
      key = scancode_ascii[scancode];
    }
  }
  
  // kbd_buffer_push((uint8_t) key);
  //
}

char keyboard_get_key() {
  
}

int keyboard_has_key() {

}