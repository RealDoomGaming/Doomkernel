#include <com1/com1.h>
#include <stdint.h>

static void init_com1() {
    // this is the function for actually initing our com1 driver

    outb(PORT + 3, 0x80);   // this enables the DLAB (Divisor Latch Access Bit) which sets the baud rate divisor
    outb(PORT + 0, 0x03);   // this sets the divisor to 3 (low byte)
    outb(PORT + 1, 0x00);   // and this is the high byte
    outb(PORT + 3, 0x03);   // these are 8 bits, no pariety and one stop bit
    outb(PORT + 2, 0xC7);   // this enables FIFO, clears the queue and sets them with a 14 byte threshold
    outb(PORT + 4, 0x0B);   // enables IRQs and also sets RTS/DSR
    outb(PORT + 4, 0x1E);   // this sets it to loopback mode
    outb(PORT + 0, 0xAE);   // then this test the serial chip (we send byte 0xAE and check later if it returns the same byte)

    // here we check is the serial is faulty
    if (inb(PORT + 0) != 0xAE) {
        return 1;
    }

    // then if the serial is not faulty we can set it to normal operation mode
    // which means not loopback with IRQs enabled and OUT#1 and OUT#2 bits enabled
    outb(PORT + 4, 0x0F);
    return 0;
}

uint8_t com1_received() {
    // this returns any input that we might receive
    return inb(PORT + 5) & 1;
}

uint8_t read_com1() {
    while (com1_received == 0) {
        // do nothing just wait
    };

    // if we got input return it
    return inb(PORT);
} 

uint8_t transmit_empty() {
    // this just returns if there is any data we might want to send
    return inb(PORT + 5) & 0x20;
}

void write_com1(uint8_t value) {
    while (transmit_empty() == 0) {
        // do nothing just wait
    }

    outb(PORT, value);
}