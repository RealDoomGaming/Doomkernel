#include <stdlib.h>
#include <stdio.h>
#include "user_code.h"

__attribute__((section(".user_text"), noinline))
void user_test_entry() {
    // this is just a test function which will get executed in ring 3

    // so in here we have a simple variable operation to confirm the user stack works
    volatile uint64_t counter = 0;

    while (1) {
        counter++;

        // and after we tested that we can return to ring 0 via triggering a system call
        __asm__ volatile ("int $0x80");
        printf("[user mode] going back into ring 0");
    }
}