#include "syscall.h"
#include <stdio.h>
#include <stdint.h>
#include <kernel/tty.h>

extern uint64_t saved_kernel_rsp;
static void (*resume_kernel_func)(void);

void syscall_init(void (*resume_func)(void))  {
    resume_kernel_func = resume_func;

    register_interrupt_handler(128, syscall_handler);
}

void syscall_handler(interrupt_frame_t *frame) {
    (void)frame;

    int syscall_num = frame->rax;

    switch (syscall_num) {
        case SYS_WRITE:
            // write syscall
            const char *buf = (const char*)frame->rdi;  // the stuff we need to print should be in the rdi register
            uint64_t len = frame->rsi;                  // and the length of the stuff we need to print should be in the rsi register
            
            // then we check if the pointer from the buffer is even in the user region
            if (frame->rdi < 0x400000 || frame->rsi > 0x800000) {
                frame->rax = (uint64_t)-1;
                break;
            }

            // if the pointer is valid we write to the terminal
            printf("[syscall] ");
            terminal_write(buf, len);

            // and then we set the return value to the length of what we wrote
            frame->rax = len;
            break;
        case SYS_EXIT:
            // exit syscall
            printf("[syscall] user programm exited with code: %d\n", (int)frame->rdi);

            // then we set the frame to be back in the kernel after the programm exited
            frame->rip = (uint64_t)resume_kernel_func;       // this is the new kernel address after we used the syscall
            frame->cs = 0x08;                           // this is the code segment the next code should run on (kernel code)       
            frame->ss = 0x10;                           // and this is the stack segment
            frame->rsp = saved_kernel_rsp;   // and this is also a refrence to the kernel stack top
            frame->rflags = 0x202;                      // and finally this is flags value, 0x2 is always reserved and 0x200 is the interrupt flag

            break;
        default:
            // if no number was given then this
            printf("[syscall] got a syscall, resuming execution");
            break;
    }
}