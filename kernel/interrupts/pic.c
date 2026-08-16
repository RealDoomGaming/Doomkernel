#include <interrupts/pic.h>

// firstly we define some constants
// each PIC (we have 2 the master and the slave) has 2 ports so we need two of everything
// these hold parameters, interrupt masks and vector offsets
#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1

#define PIC_EOI 0x20
#define ICW1_INIT 0x10      // this puts the PIC into init mode
#define ICW1_ICW4 0x01      // this signals that a 4th init command will follow
#define ICW4_8086 0x01      // this tells the PIC to operate in x86/8086 mode

// then we have to define some inline assembly handlers which we can use later

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

static inline void io_wait() {
    // this is used because writes to an io port execute faster then the internal Pics circuitry can process them
    // so writing a dummy byte to port 0x80 forces the cpu to pause for a second
    outb(0x80, 0);    
}

// now we go to the actual function in this file
void pic_remap(int offset1, int offset2) {
    // the problem is that on modern processors the first 31 vectors are reserved for CPU Exceptions
    // but that created a conflict with IBMS Pc hardware design so we have to remap here

    // firstly we save the existing state
    uint8_t mask1 = inb(PIC1_DATA);
    uint8_t mask2 = inb(PIC2_DATA);

    // then we start the inilization and send 0x11 (ICW1_INIT | ICW1_ICW4) to both command ports
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    // then after that we have to set the base offset vectors
    // so irq 0 maps to  offset1 + 0
    // irq1 maps to offset1 + 1
    // irq8 maps to offset2 + 0
    // and so on...
    outb(PIC1_DATA, offset1);
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();

    // after that we configure cascading by
    // making the master pic receive bitmask 4 which tells it that irq 2 is connected to the slave pic
    outb(PIC1_DATA, 4);
    io_wait();
    // and telling the slave pic that its cascade identity is irq 2
    outb(PIC2_DATA, 2);
    io_wait();

    // and we also tell both pics to operate in 8086/88 architecture mode
    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // and finally we restore what we saved from the pics before
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void pic_send_eoi(uint8_t irq) {
    // this function gets called when a device triggers and interrupt
    // and it is needed because we need to send and eoi command else the pci thinks the interrupt is still running and will freeze everything else
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    outb(PIC1_COMMAND, PIC_EOI);
}

void pic_set_mask(uint8_t irq) {
    // this enables the mask (disables the irq)
    uint16_t port = irq < 8 ? PIC1_DATA : PIC2_DATA;     // so we know if its a slave or master
    uint8_t irq_line = irq < 8 ? irq : irq - 8;          // converts full irq numbers to chip irq indexes
    uint8_t value = inb(port) | (1 << irq_line);         // and this reads the current register and sets the corresponding bit with the OR operation
    outb(port, value);                                   // and this finally writes updated masks
}

void pic_clear_mask(uint8_t irq) {
    // this basically the same as setting the mask the only difference is
    uint16_t port = irq < 8 ? PIC1_DATA : PIC2_DATA;
    uint8_t irq_line = irq < 8 ? irq : irq - 8;
    // here where we clear the target with the AND
    uint8_t value = inb(port) & ~(1 << irq_line);
    outb(port, value);
}