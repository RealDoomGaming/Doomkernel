#include <interrupts/pic.h>

// firstly we define some constants
// each PIC (we have 2 the master and the slave) has 2 ports so we need two of everything
// these hold parameters, interrupt masks and vector offsets
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMANDS 0xA0
#define PIC2_DATA 0xA1

#define PIC1_EOI 0x20
#define ICW1_INIT 0x10      // this puts the PIC into init mode
#define ICW1_ICW4 0x01      // this signals that a 4th init command will follow
#define ICW4_8086 0x01      // this tells the PIC to operate in x86/8086 mode

// then we have to define some inline assembly handlers which we can use later

static inline void outb(uint16_t port, uint8_t values) {
    // outb is for writing byte to the port
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    // inb is for readin byte from a port and then we return them
    uint8_t ret;
    __asm__ volatile("inb %0, %1" : : "a"(ret), "Nd"(port));
    return ret;
}

static inline void io_wait() {
    // this is used because writes to an io port execute faster then the internal Pics circuitry can process them
    // so writing a dummy byte to port 0x80 forces the cpu to pause for a second
    outb(0x80, 0);    
}