#include <stdlib.h>
#include <string.h>
#include <stddef.h>

void free (void* ptr) {
    // in free we get a ptr to a memory address
    // now since we have our handy allocate struct infront of each allocated memory block we can just subtract the sizeof the struct and get the struct
    // then we can set it to free and also clear the memory which was used

    if (!ptr) {
        return;
    }

    alloc_t *alloc = (alloc_t*)((uint8_t*)ptr - sizeof(alloc_t));

    if (alloc->status == 0) {
        return;
    }

    alloc->status = 0;

    memory_used -= sizeof(alloc_t) + alloc->size;

    memset((void*)((uint64_t)alloc + sizeof(alloc_t)), 0, alloc->size);
}