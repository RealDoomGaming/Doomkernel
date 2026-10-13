#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stdint.h>
#include <interrupts/isr.h>

#define SYS_WRITE 1
#define SYS_EXIT 60

void syscall_init(uint64_t, void (*resume_func)(void))
void syscall_handler(interrupt_frame_t *);

#endif