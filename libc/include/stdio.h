#ifndef _STDIO_H
#define _STDIO_H

// we need to basically include this everywhere in every file in the libc
#include <sys/cdefs.h>

// EOF gets returned when any of the 3 functions fails so we need to define it here
#define EOF (-1)

int printf(const char*, ...);
int putchar(int);
int puts(const char*);

#endif