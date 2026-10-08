#include <stdlib.h>
#include <stdio.h>
#include "user_code.h"

__attribute__((section(".user_text"), noinline))
void user_test_entry() {
    // this is just a test function which will get executed in ring 3

    // so in here we have a simple variable operation to confirm the user stack works
    volatile uint64_t counter = 0;

    while (counter < 5) {
        counter++;

        __asm__ volatile (
            "int $0x80"
            :
            : "D" ((uint64_t)counter)
            : "memory"
        );
    }

    for (;;) {
        __asm__ volatile ("pause");
    }
}