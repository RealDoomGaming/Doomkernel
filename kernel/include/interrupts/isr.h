#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>

// we need a register for the interrupts
typedef struct {
    uint32_t ds;            // this is for the data segment selector
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // these will all be pushed by the pusha instruction
    uint32_t int_no, err_code;     // these are the interrupt numbers and the error code
    uint32_t eip, cs, eflags, useresp, ss; // these will all be pushed by the cpu automatically
} interrupt_frame_t;

// this is so we accept a copy of the register instead of the actual register
typedef void (*isr_handler_t)(interrupt_frame_t *frame);
// this function is for registerting the different handlers
void register_interrupt_handler(uint8_t interrupt, isr_handler_t handler);

#endif