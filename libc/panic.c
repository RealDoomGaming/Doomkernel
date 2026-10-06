#include <panic.h>
#include <stdio.h>

void panic(const char *msg, const interrupt_frame_t *frame, const char* file, const int line) {
    __asm__ volatile ("cli");

    printf("*******************KERNEL PANIC*******************\n");
    printf("Error: %s\n", msg);
    printf("File: %s (Line: %d)\n", file, line);

    if (frame != NULL) {
        printf("****CPU****\n");
        printf("INT NO: %d\n", frame->int_no);
        printf("ERR CCODE: %x\n", frame->error_code);
        printf("RIP: %x\n", frame->rip);
        printf("CS: %x\n", frame->cs);
        printf("RING: %d\n", frame->cs & 3);
        printf("RFLAGS: %x\n", frame->rflags);
        printf("RSP: %x\n", frame->rsp);
        printf("SS: %x\n", frame->ss);

        // if page fault error then we read cr2
        if (frame->int_no == 14) {
            uint64_t cr2;
            __asm__ volatile ("mov %%cr2, %0" : "=r"(cr2));
            printf("CR2: %x\n", cr2);
        }
    }

    for (;;) __asm__ volatile ("hlt");
}