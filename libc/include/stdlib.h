#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/cdefs.h>
#include <stddef.h>

void* malloc(size_t);
void free (void*);
void* calloc(size_t , size_t);
void* realloc(void*, size_t);

#endif