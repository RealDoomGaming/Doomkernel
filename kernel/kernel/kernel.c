#include <stdint.h>

#include <kernel/tty.h>

/*
void put_char(const char c) {
    // when we want to print a char we firstly need to check if its a newline or a carriage return
    // and only then can we combine the character and the color into a 16 bit entry and put it inot the VGA buffer

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else if (c == '\r') {
        cursor_x = 0;
    } else {
        // here we combine the char and the color of the char into a 16 bit entry which we will use to clear the screen later
        uint16_t combined = c | (WHITE_ON_BLACK << 8);
        int index = (cursor_y * VGA_WIDTH) + cursor_x;

        VGA[index] = combined;

        cursor_x++;
        if (cursor_x > VGA_WIDTH) {
            cursor_x = 0;
            cursor_y++;
        }
    }

    if (cursor_y > VGA_HEIGHT) {
        clear_screen();
        cursor_y = 0;
    }
}

void print(const char msg[], const int msg_length) {
    for (int i = 0; i < msg_length; i++) {
        put_char(msg[i]);
    } 
}*/

void kernel_main() {
    // first thing we do is init the terminal
    terminal_init();

    /*
    // this is our success message string
    const char success_msg[] = "Successfully loaded the kernel. \n";
    const int succ_msg_length = 33;

    print(success_msg, succ_msg_length);

    // another message for testing the cursor
    const char cursor_test_msg[] = "This should be in the next line. \n";
    const int cursor_test_msg_length = 34;

    print(cursor_test_msg, cursor_test_msg_length);
    */

    // while loop so the cpu doesnt run off into memory junk
    while (1) {
        __asm__ volatile("hlt");
    }
}