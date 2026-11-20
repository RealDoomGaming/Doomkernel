#include <com-ports/com1.h>
#include <stdint.h>
#include <string.h>
#include <io/ports.h>

uint8_t is_serial_available = 0;

void write_string(const char *str) {
    // this function is for writing a string one byte at a time to the com1

    // if we didnt get a string then return instantly
    if (!str) {
        return;
    }

    // here we define and index, a current character and the current character in decimal
    uint64_t i = 0;
    char current_char = str[i];
    uint8_t current = (uint8_t) str[i];

    // then while our current character isnt the end of the string
    while (current_char != '\0') {
        // we check if it is the end of a line because then we need to send \r first
        if (current_char == '\n') {
            write_com1('\r');
        }

        // we send the current decimal byte thingi
        write_com1(current);

        // and then we increment everything
        i++;
        current_char = str[i];
        current = (uint8_t) str[i];
    }
}

uint8_t init_com1() {
    // this is the function for actually initing our com1 driver

    outb(COM1_PORT + 1, 0x00);   // we disable all interrupt with this
    outb(COM1_PORT + 3, 0x80);   // this enables the DLAB (Divisor Latch Access Bit) which sets the baud rate divisor
    outb(COM1_PORT + 0, 0x03);   // this sets the divisor to 3 (low byte)
    outb(COM1_PORT + 1, 0x00);   // and this is the high byte
    outb(COM1_PORT + 3, 0x03);   // these are 8 bits, no pariety and one stop bit
    outb(COM1_PORT + 2, 0xC7);   // this enables FIFO, clears the queue and sets them with a 14 byte threshold
    outb(COM1_PORT + 4, 0x0B);   // enables IRQs and also sets RTS/DSR
    outb(COM1_PORT + 4, 0x1E);   // this sets it to loopback mode
    outb(COM1_PORT + 0, 0xAE);   // then this test the serial chip (we send byte 0xAE and check later if it returns the same byte)

    // here we check is the serial is faulty
    if (inb(COM1_PORT + 0) != 0xAE) {
        is_serial_available = 1;
        return 1;
    }

    // then if the serial is not faulty we can set it to normal operation mode
    // which means not loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled
    outb(COM1_PORT + 4, 0x0F);
    return 0;
}

uint8_t com1_received() {
    // this returns any input that we might receive
    return inb(COM1_PORT + 5) & 1;
}

uint8_t read_com1() {
    while (com1_received() == 0) {
        // do nothing just wait
    };

    // if we got input return it
    return inb(COM1_PORT);
} 

uint8_t transmit_empty() {
    // this just returns if there is any data we might want to send
    return inb(COM1_PORT + 5) & 0x20;
}

void write_com1(uint8_t value) {
    while (transmit_empty() == 0) {
        // do nothing just wait
    }

    outb(COM1_PORT, value);
}