#include <panic.h>
#include <stdio.h>

void panic(const char* format, const char* file, const int line) {
    char *panic_msg = "****KERNEL PANIC**** \n At file: %s. \n";

    printf(panic_msg, file);
    printf("%s\n", format);

    for (;;) __asm__ volatile ("cli; hlt");
}