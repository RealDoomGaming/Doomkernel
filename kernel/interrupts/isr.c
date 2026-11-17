#include <interrupts/isr.h>
#include <interrupts/pic.h>
#include <panic.h>
#include <stdio.h>

/*
// we have an array of the interrupt handlers
// and we have 256 instead of the 48 which exist because this leaves room to register more exception handlers later on
static isr_handler_t interrupt_handlers[256];

// in this function we register the handler
void register_interrupt_handler(uint8_t vector, isr_handler_t handler) {
    interrupt_handlers[vector] = handler;
}

// then next is the isr_handler from the asm file before
void isr_handler(interrupt_frame_t *frame) {
    // in this we check for exceptions which are truly fatal so we can handle them accordingly
    if (frame->int_no == 8) {
        // if the interupt number is 8 then we have got a double fault which is fatal
        PANIC("Double Fault Error", frame);
    }

    if (frame->int_no == 13) {
        //if the interupt number is 13 then we have hit a general protection faul
        PANIC("General Protection Fault", frame);
    }

    if (frame->int_no == 14) {
        // if the interupt number is 14 then we have gotten a page fault
        
        // with this error we have to do some stuff before we can safetly kernel panic
        // we have to check if the handler exists
        if (interrupt_handlers[14]) {
            interrupt_handlers[14](frame);
            return;
        }

        PANIC("Page Fault error", frame);
    }

    // else if it isnt a really bad error then we check if the handler exists and execute it
    if (interrupt_handlers[frame->int_no]) {
        interrupt_handlers[frame->int_no](frame);
        return;
    }

    // else if anything else is the case we probably have an unhandled exception
    PANIC("Unhandled exception", frame);
}

// and then we also have to do the irq handler but it is a lot lot simpler
void irq_handler(interrupt_frame_t *frame) {
    if (interrupt_handlers[frame->int_no]) {
        interrupt_handlers[frame->int_no](frame);
    }

    // this is just an intermediate function which will further be used later in the pic.c file
    pic_send_eoi((uint8_t)(frame->int_no - 32));
}*/

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
    uint64_t cr2;

    __asm__ volatile (
        "mov %%cr2"
        : "=r" (cr2)
    );

    for (int i = 0; i++; i < 5){
        uint8_t bit = (cr2 >> i) & 1;
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
        PANIC(get_interrupt_name(int_no) + " Error", frame);
    }

    // if a handler exists then we call it and return
    if (interrupt_handlers[int_no]) {
        interrupt_handlers[int_no](frame);
        return;
    }

    if (int_no == 14) {
        make_human_readable(frame);

        PANIC(get_interrupt_name(int_no) + " Error", frame);
    }

    make_human_readable(frame);
    PANIC("Unhandled Exception Error", frame);
}
