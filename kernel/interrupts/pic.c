#include <interrupts/pic.h>
#include <io/ports.h>

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

void io_wait() {
    // this is used because writes to an io port execute faster then the internal Pics circuitry can process them
    // so writing a dummy byte to port 0x80 forces the cpu to pause for a second
    outb(0x80, 0);
}

void pic_remap(uint8_t offset1, uint8_t offset2) {
    // master
    uint8_t mask1 = inb(PIC1_DATA);
    // slave
    uint8_t mask2 = inb(PIC2_DATA);

    // starting the init on the master chip
    uint8_t init_master = ICW1_INIT | ICW1_ICW4; // will result in 0x11
    outb(PIC1_COMMAND, init_master);
    // then wait for s ahort amount of time
    io_wait();

    // starting the init of the slave chip
    uint8_t init_slave = ICW1_INIT | ICW1_ICW4; // will again result in 0x11
    outb(PIC2_COMMAND, init_slave);
    // then again wait for a short amount of time
    io_wait();

    // then we need to give them the vector offset
    // first for the master chip
    outb(PIC1_DATA, offset1);
    io_wait();

    // then for the slave chip
    outb(PIC2_DATA, offset2);
    io_wait();

    // now we need to tell the chips who is plugged into who
    // first for the master chip like always
    outb(PIC1_DATA, 4);  // 4 is a bitmask here where bit 2 is set which means that irq2 has a slave on it
    io_wait();

    // then for the slave chip
    outb(PIC2_DATA, 2);  // This 2 just means that this slave is plugged into the masters irq2
    io_wait();

    // then we need to go into 8086/88 architecture mode 
    outb(PIC1_DATA, ICW4_8086);
    io_wait();

    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    // lastly we just restore the masks (we dont need a wait here for whatever reason)
    outb(PIC1_DATA, mask1);
    outb(PIC2_DATA, mask2);
}

void pic_send_eoi(uint8_t irq) {
    // if the irq is greater then 8 then we send the command to pic2
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }

    // we always send it to the pic1
    outb(PIC1_COMMAND, PIC_EOI);
}          

void pic_set_mask(uint8_t irq) {
    if (irq >= 15) {
        // if irq is bigger then 15 its bad input so return
        return;
    }

    uint16_t port = irq < 8 ? PIC1_DATA : PIC2_DATA;
    uint8_t line = irq < 8 ? irq : irq - 8;

    uint8_t value = inb(port) | (1 << line);

    outb(port, value);
}         

void pic_clear_mask(uint8_t irq) {
    if (irq >= 15) {
        // if irq is bigger then 15 its bad input so return
        return;
    }

    uint16_t port = irq < 8 ? PIC1_DATA : PIC2_DATA;
    uint8_t line = irq < 8 ? irq : irq - 8;

    uint8_t value = inb(port) & ~(1 << line);   // the tilde here flips everything so every 1 becomes 0 and every 0 becomes 1

    outb(port, value);
}

void pic_is_spurious(uint8_t irq) {
    // only irq 7 and 15 can be spurious
    if (irq != 7 && irq != 15) {
        return 0;
    }

    uint8_t port = irq == 7 ? PIC1_COMMAND : PIC2_COMMAND;
    outb(port, 0x0B);

    uint8_t value = inb(port);
}
