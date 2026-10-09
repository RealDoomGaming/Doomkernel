#include <stdint.h>
#include "user_code.h"
#include "syscall_nums.h"

__attribute__((always_inline))
static inline uint64_t system_call(uint64_t num, uint64_t a, uint64_t b) {
    uint64_t ret;

    // a = rax
    // D = rdi
    // S = rsi
    __asm__ volatile (
        "int $0x80"
        : "=a" (ret)
        : "a" (num), "D"(a), "S"(b)
        : "memory"
    );

    return ret;
}

__attribute__((section(".user_text"), noinline))
void user_test_entry() {
    // this is just a test function which will get executed in ring 3

    // we have a msg here which is the buffer to print later
    volatile char msg[8] = {'c', 'o', 'u', 'n', 't', ' ', '0', '\n'};
    // but we still need our vounter
    volatile uint64_t counter = 0;

    while (counter < 5) {
        counter++;

        // then every count we override the 6th entry in our msg buffer
        msg[6] = (char)('0' + counter);

        // and then we do the syscall
        system_call(SYS_WRITE, (uint64_t)msg, 8);
    }

    // then after we are done with the counter we want to call a sysexit
    system_call(SYS_EXIT, 0, 0);

    // this is technically never reacher but we will still keep this here
    for (;;) {
        __asm__ volatile ("pause");
    }
}