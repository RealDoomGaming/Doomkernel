#include <stdlib.h>
#include <string.h>

void* calloc(size_t amount, size_t size) {
    // calloc is simmilar to malloc only that the size we need to allocate is amount * size instead of only the size
    // so refer to malloc for any comments if anything is unclear or something
    
    if (size <= 0) {
        return 0;
    }

    if (amount <= 0) {
        return 0;
    }

    // if we have an amount but the size times the amount would be too big
    if (amount != 0 && size > (SIZE_MAX / amount)) {
        return 0; 
    }

    uint64_t total_size = amount * size;

    uint8_t *memory_loc = (uint8_t*) heap_beginning;

    if (heap_beginning == 0) {
        // not inited yet
        return 0;
    }

    while ((uint64_t) memory_loc < last_alloc) {

        alloc_t *alloc = (alloc_t*)memory_loc;

        if (!alloc->size) {
            goto new_alloc;
        }

        if (alloc->status == 1) {
            memory_loc += alloc->size;
            memory_loc += sizeof(alloc_t);

            continue;
        }

        if (alloc->size >= (uint64_t) total_size) {

            if (alloc->size - total_size > sizeof(alloc_t)) {
                alloc_t *remainder = (alloc_t*)((uint8_t*)alloc + sizeof(alloc_t) + total_size);
                remainder->status = 0;
                remainder->size = alloc->size - total_size - sizeof(alloc_t);
                alloc->size = total_size;
            }

            alloc->status = 1;
            memory_used += total_size + sizeof(alloc_t);
            memset((void*)((uint64_t)alloc + sizeof(alloc_t)), 0, total_size);

            return (void*)(memory_loc + sizeof(alloc_t));
        }

        memory_loc += alloc->size;
        memory_loc += sizeof(alloc_t);
    }

    new_alloc:;
    if (last_alloc + total_size + sizeof(alloc_t) >= heap_end) {
        return 0;
    }

    alloc_t *new = (alloc_t*) memory_loc;
    new->status = 1;
    new->size = total_size;

    last_alloc += total_size + sizeof(alloc_t);

    memory_used += total_size + sizeof(alloc_t);

    memset((void*)((uint64_t)new + sizeof(alloc_t)), 0, total_size);

    return (void*)((uint64_t)new + sizeof(alloc_t));
}