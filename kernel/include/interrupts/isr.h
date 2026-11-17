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

// if bit n is not set (so it 0)
static char *bit_zero[5] = {
    "page not present",
    "read",
    "supervisor mode",
    "no reserved-bit problem",
    "not an instruction fetch"
};
// if bit n is set (so its 1)
static char *bit_one[5] = {
    "protection violation",
    "write",
    "user mode",
    "reserved bit set in a page table entry",
    "instruction fetch"
};

// table of all interrupt errors which can occur
const char *exception_errors[32] = {
    "Divide-By-Zero Error",
    "Debug",
    "Non-Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

#endif