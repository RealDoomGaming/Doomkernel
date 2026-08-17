#ifndef _STDLIB_H
#define _STDLIB_H

#include <sys/cdefs.h>
#include <stddef.h>
#include <stdint.h>

// we need to align our memory chuncks to 16 bytes in order to keep our heap aligned properly and not nock it off
#define ALLOC_ALIGN 16
// this function will align whatever we give it to what ALLOC_ALIGN has
// ((x) + (ALLOC_ALIGN - 1)) -> when we add ALLOC_ALIGN - 1 to x it would push x past the ALLOC_ALIGN boundary and if x is already aligned it would not increase it enough 
// to push it past
// ~(uint64_t)(ALLOC_ALIGN - 1) -> this would create a bitmask which zeros out all lower bits and then with ~ we just invert it so
// the bitmask ends in only zeros
// and then ANDing thos both things would push the new x we got from before down to the nearest alignment for ALLOC_ALIGN
#define ALIGN_UP(x) (((x) + (ALLOC_ALIGN - 1)) & ~(uint64_t)(ALLOC_ALIGN - 1))

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

// after our header struct we have to check at compile time if the struct is even a multiple of our ALLOC_ALIGN so it fits with the memory chuncks we got
_Static_asser(sizeof(alloc_t) % ALLOC_ALIGN == 0, "alloc_t must be a multiple of ALLOC_ALIGN");

void* malloc(size_t);
void free (void*);
void* calloc(size_t , size_t);
void* realloc(void*, size_t);

void memory_init(uint64_t, mmap_entry_t*, uint16_t);

#endif