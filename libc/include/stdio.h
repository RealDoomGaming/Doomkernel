#ifndef _STDIO_H
#define _STDIO_H

// we need to basically include this everywhere in every file in the libc
#include <sys/cdefs.h>

// EOF gets returned when any of the 3 functions fails so we need to define it here
#define EOF (-1)

// we need to do this here because if we ever do anything with c++ it should be able to call these and if we dont have this here then it can see them but wont find the functions
#ifdef __cplusplus
extern "C" {
#endif

int printf(const char*, ...);
int putchar(int);
int puts(const char*);

#ifdef __cplusplus
}
#endif

#endif