#include <stdlib.h>
#include <string.h>
#include <stddef.h>

alloc_t* find_before(alloc_t *alloc) {
    // in this function we basically start at the heaps beginning and move up until we find that the next alloc is the one we got as a parameter

    alloc_t *cur = (alloc_t*)((uint8_t*)heap_beginning);
    alloc_t *next = (alloc_t*)((uint8_t*)cur + sizeof(alloc_t) + cur->size);

    if (cur == alloc) {
        return 0;
    }

    while (next != alloc) {
        cur = next;
        next = (alloc_t*)((uint8_t*)cur + sizeof(alloc_t) + cur->size);
    }

    return cur;
}

void free(void* ptr) {
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

    // after we have set the status of the memory chunck to unused we can check if there are any 
    // adjacent memory chuncks which are also unused and merge them if they are
    // the next memory chunk
    alloc_t *next = (alloc_t*)((uint8_t*)ptr + alloc->size);
    alloc_t *before = find_before(alloc);

    // firstly add the next one
    if ((uint64_t)next < last_alloc && next->status == 0) {
        alloc->size += sizeof(alloc_t) + next->size;
    }

    // and then we merge the whole block backwards if the one before is free
    if (before && before->status == 0) {
        before->size += sizeof(alloc_t) + alloc->size;
        alloc = before;
    }

    memset((void*)((uint64_t)alloc + sizeof(alloc_t)), 0, alloc->size);
}