#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>

// we need a register for the interrupts
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;  // this is for the data segment selector
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;     // these will all be pushed by the pusha instruction
    uint64_t int_no, err_code;                      // these are the interrupt numbers and the error code
    uint64_t rip, cs, rflags, rsp, ss;              // these will all be pushed by the cpu automatically
} interrupt_frame_t;

// this is so we accept a copy of the register instead of the actual register
typedef void (*isr_handler_t)(interrupt_frame_t *frame);
// this function is for registerting the different handlers
void register_interrupt_handler(uint8_t interrupt, isr_handler_t handler);

#endif