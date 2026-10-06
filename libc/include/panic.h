#ifndef _PANIC_H
#define _PANIC_H
#include <interrupts/isr.h>

void panic(const char*, const interrupt_frame_t*, const char*, const int);
void panic_msg(const char*, const char*, const int);

#define PANIC(msg, frame) panic(msg, frame, __FILE__, __LINE__)
#define PANIC_MSG(msg) panic_msg(msg, __FILE__, __LINE__)

#endif