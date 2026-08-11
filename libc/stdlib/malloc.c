#include <stdlib.h>
#include <string.h>

// we need these for specific things later, I think the names explain good engough for what we will need them
uint32_t heap_beginning;
uint32_t heap_end;
uint32_t last_alloc;
uint32_t memory_used;

void memory_init(uint32_t kernel_end) {
    // the heap bedgins where the kernel end just with a bit of a buffer between them
    heap_beginning = kernel_end + 0x1000;
    // then the heap ends at 0x400000 which is 4MB, later we can make a function which grows this heap further until we hit the limit of the ram
    heap_end = 0x400000
    // the last alloc at the start is of course the beginning of the heap
    last_alloc = heap_beginning;
    // and the memory used at the start is 0;
    memory_used = 0;
}

void* malloc(size_t size) {
    // in this function we will just allocate memory for the use by using the struct we defined in the header file to indicate where allocated memory starts
    
    // if the size is 0 or lower then we can just return as we wont allocate anything
    if (size <= 0) {
        return 0;
    }

    // then else if we actually have a size to allocate
    // we need to know where our heap began
    uint8_t *memory_loc = (uint8_t*) heap_beginning;
    // and then loop through our existing blocks so if we find a free one which has the required size we can give it back
    while ((uint32_t) memory_loc < last_alloc) {
        // we get the block where we are at right now
        alloc_t *alloc = (alloc_t*)memory_loc;

        // if the alloc doesnt exist or has no size then we know that we are at the end of the allocation
        if (!alloc->size) {
            goto new_alloc;
        }

        // elsse if the allocation has size we check if its even available
        if (alloc->status == 1) {
            // we know it has been allocated so we need to move further through the memory
            memory_loc += alloc->size;
            memory_loc += sizeof(alloc_t);
            // and then we just skip to the next block with continue
            continue;
        }

        // else if we know its free we can check if the size fits for what we got from the parameters
        if (alloc->size >= (uint32_t) size) {
            // if the size fits for us we can actually use this memory location

            // firstly we set the current structs availability to 1 so its marked as used
            alloc->status = 1;
            // we could cut it down if we know its too big and make a new struct for later use but I am too lazy to do that right now :)
            memory_used += size + sizeof(alloc_t);
            // and then we just return a pointer to where the actual memory starts after the struct
            return (void*)(memory_loc + sizeof(alloc_t));
        }

        // and else if it is free but the space is too small for our size then we just continue to the next one
        memory_loc += alloc->size;
        memory_loc += sizeof(alloc_t);
    }

    // here we define a label which is called if we couldnt find anything free which fits us or if we were at the end of the already allocated memory
    new_alloc:;
    if (last_alloc + size + sizeof(alloc_t) >= heap_end) {
        // we are out of memory and probably need to panic here (kernel panic) but I havent implemented that yet
        return -1;
    }

    // now we just have to make a new alloc struct with the corressponding values
    // then we have to also change the last alloc and return the new pointer to the user
    alloc_t *new = (alloc_t*) memory_loc;
    new->status = 1;
    new->size = size;

    last_alloc += size + sizeof(alloc_t);

    // then we also have to add the new used memory to our used memory counter
    memory_used += size + sizeof(alloc_t);

    // and we clear the memory by setting everything in it to 0
    memset((void*)((uint32_t)new + sizeof(alloc_t)), 0, size);

    // and then we just return a pointer to that free memory
    return (void*)((uint32_t)new + sizeof(alloc_t));
}