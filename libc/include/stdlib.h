#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

// we need this to know where the allocated memory starts later
typedef struct {
	uint8_t status;
	uint32_t size;
} alloc_t;

void* malloc(size_t);
void free (void*);
void* calloc(size_t , size_t);
void* realloc(void*, size_t);

memory_init(uint32_t);

#endif