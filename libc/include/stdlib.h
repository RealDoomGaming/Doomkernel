#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

// this is the struct for memory entries so we can init the heap properly
typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;
    uint32_t acpi_ext;
} __attribute__((packed)) mmap_entry_t;

// same with this, bios type 1 indicates that the memory is usable ram which we can use for the heap
#define MMAP_USABLE 1


// we need these for specific things later, I think the names explain good engough for what we will need them
extern uint64_t heap_beginning;
extern uint64_t heap_end;
extern uint64_t last_alloc;
extern uint64_t memory_used;

// we need this to know where the allocated memory starts later
typedef struct {
	uint8_t status;
	uint64_t size;
} alloc_t;

void* malloc(size_t);
void free (void*);
void* calloc(size_t , size_t);
void* realloc(void*, size_t);

void memory_init(uint64_t);

#endif