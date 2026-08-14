#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <interrupts/isr.h>
#include <kernel/tty.h>

extern uint64_t kernel_end;
// for the timer we also need a tick counter
static volatile uint32_t tick_count = 0;

// in this function we define what happens when we get a breakpoint
void breakpoint_handler(interrupt_frame_t *frame) {
    (void)frame;
    printf("[handler] breakpoint caught, resuming execution\n");
}

// in this function we test a timer
void timer_handler(interrupt_frame_t *frame) {
    (void)frame;
    tick_count++;
    if (tick_count % 100 == 0) {
        printf("[timer] tick %u\n", tick_count);
    }
}

// and in this we actually register it
void register_breakpoint_handler() {
    register_interrupt_handler(3, breakpoint_handler);
}

// and we register our timer
void register_timer_handler() {
    register_interrupt_handler(32, timer_handler);
}


void kernel_main() {
    // first thing we do is init the terminal
    terminal_init();
    // just a msg
    printf("[terminal] cursors and color set, buffer set to VGA and screen cleared\n");

    // here we init the entire interrupt stuff
    idt_init();
    pic_remap(0x20, 0x28);
    __asm__ volatile("sti");
    // also just a msg
    printf("[interrupts] IDT loaded, PIC remapped, interrupts enabled\n");

    // for testint purposes we register the handlers
    register_breakpoint_handler();
    register_timer_handler();

    // here we make a small test to see if the interrupts work
    printf("[test] triggering breakpoint\n");
    __asm__ volatile("int3");
    printf("[test] we are still alive (no kernel panic)\n");

    // now after we tested the exception from the cpu we have to test something which goes through
    // the whole hardware IRQ pipeline
    


    // then we init the memory
    memory_init((uint64_t)&kernel_end);
    // also just a msg
    printf("[memory] heap beginning and end was set\n");

    // printing with our custom printf function :DD
    printf("Successfully booted into the kernel!\n");

    // while loop so the cpu doesnt run off into memory junk
    while (1) {
        __asm__ volatile("hlt");
    }
}