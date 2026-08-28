#include <timer/pit.h>
#include <io/ports.h>
#include <interrupts/isr.h>
#include <interrupts/pic.h>
#include <task/task.h>

// this is the fixed base frequency of the PIT chip, so you cant really choose it
#define PIT_BASE_FREQUENCY 1193182
// this is the port on the chip where we send then config bytes to
#define PIT_COMMAND 0x43
// and then finally the port on the chip where we will write the divisor to
#define PIT_CHANNEL0 0x40

// the actual storage for the counter pit.h declares, it is volatile since the
// irq0 handler bumps it behind the back of whatever code is currently running
volatile uint64_t ticks = 0;

interrupt_frame_t kernel_frame_template;
static uint8_t scheduling_enabled = 0;

static inline void io_wait() {
    // this is used because writes to an io port execute faster then the internal Pics circuitry can process them
    // so writing a dummy byte to port 0x80 forces the cpu to pause for a second
    outb(0x80, 0);    
}

static void pit_handler(interrupt_frame_t *frame) {
    ticks++;
    kernel_frame_template = *frame;

    // if scheduling is enabled then we can schedule it
    if (scheduling_enabled) {
        schedule(frame);
    }
}

// we set scheduling enabled here
void scheduler_enable(void) {
    scheduling_enabled = 1;
}

void timer_init(uint16_t frequency) {
    // we need the divisor later to be able to define how many times per second an interrupt is supposed to be sent
    uint16_t divisor = (uint16_t)(PIT_BASE_FREQUENCY / frequency);
    ticks = 0;

    // after we have got the divisor we need to send some configs to the chip
    // why we choose 0x36 here out of all numbers:
    // we need the bit sequence 00 11 011 0 = 0x36
    // the 00 are for channel 0
    // then the 11 are for sending the low byte and then the high byte
    // then the 011 are for sending mode 3, which means square waves repeating forever
    // and the last bit 0 is for sending binary
    outb(PIT_COMMAND, 0x36);
    io_wait();

    // after sending the config to the byte we can also send the divisor
    // we have to do something special with this one because the PIT_CHANNEL0 can only recieve 8 bits
    // and we set it so firstly it expects the low byte and then the high byte
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
    io_wait();

    // and then we have to register the handler for when an interrupt gets fired
    // 32 is the IRQ0
    register_interrupt_handler(32, pit_handler);

    // then we have to unmask IRQ0 so we can actually register the handler
    pic_clear_mask(0);
}