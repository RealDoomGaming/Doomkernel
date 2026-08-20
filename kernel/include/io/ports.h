#ifndef _PORTS_H
#define _PORTS_H

#include <stdint.h>

// we have to define the out and in cpu insutrctions here so we can use them anywhere because before they where only in the pic c file
static inline void outb(uint16_t port, uint8_t value) {
    // outb is for writing byte to the port
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    // inb is for readin byte from a port and then we return them
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}


#endif