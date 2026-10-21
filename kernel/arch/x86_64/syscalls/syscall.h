#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stdint.h>
#include <interrupts/isr.h>

#define USER_CODE_BASE 0x400000ULL
#define USER_STACK_TOP 0x800000ULL

#define SYS_WRITE 1
#define SYS_EXIT 60

void syscall_init(void (*resume_func)(void));
void syscall_handler(interrupt_frame_t *);

#endif