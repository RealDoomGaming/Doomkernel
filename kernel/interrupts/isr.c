#include <interrupts/isr.h>
#include <interrupts/pic.h>
#include <panic.h>
#include <stdio.h>
#include <stdint.h>

// this is a table for all of the interrupt handlers
// we officially need to support 48 but since we could want to register more later ourselfs we set it to 256 for now
static isr_handler_t interrupt_handlers[256];

void register_interrupt_handler(uint8_t vector, isr_handler_t handler) {
    // stores the pointer to the handler at the vector index
    interrupt_handlers[vector] = handler;
}

char *get_interrupt_name(uint64_t int_no) {
    return exception_errors[int_no];
}

static void make_human_readable(interrupt_frame_t *frame) {
    uint64_t int_err = frame->err_code;

    /*__asm__ volatile (
        "mov %%cr2, %0"
        : "=r" (cr2)
    );*/

    for (int i = 0; i < 5; i++){
        uint8_t bit = (int_err >> i) & 1;
        char *meaning;

        if (bit == 1) {
            meaning = bit_one[i];
        } else if (bit == 0) {
            meaning = bit_zero[i];
        }

        printf("Bit %d is %d, meaning: %s", i, bit, meaning);
    }
}

void isr_handler(interrupt_frame_t *frame) {
    uint64_t int_no = frame->int_no;

    // handle every interrupt number code
    if (int_no == 8) {
        PANIC(get_interrupt_name(int_no), frame);
    }

    // if a handler exists then we call it and return
    if (interrupt_handlers[int_no]) {
        interrupt_handlers[int_no](frame);
        return;
    }

    if (int_no == 14) {
        make_human_readable(frame);

        PANIC(get_interrupt_name(int_no), frame);
    }

    PANIC("Unhandled Exception Error", frame);
}

void irq_handler(interrupt_frame_t *frame) {
    uint64_t int_no = frame->int_no;

    // if we have an interrupt registered we can call it
    if (interrupt_handlers[int_no]) {
        interrupt_handlers[int_no](frame);
    }

    // here we call this (look in pic.c for futher documentations)
    pic_send_eoi((uint8_t) int_no-32);
}
