#ifndef _PANIC_H
#define _PANIC_H

void panic(const char*, const interrupt_frame_t*, const char*, const int);

#define PANIC(msg, frame) panic(msg, frame, __FILE__, __LINE__)

#endif