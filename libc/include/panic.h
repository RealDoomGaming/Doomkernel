#ifndef _PANIC_H
#define _PANIC_H


void panic(const char*, const char*, const int);

#define PANIC(msg) panic(msg, __FILE__, __LINE__)

#endif