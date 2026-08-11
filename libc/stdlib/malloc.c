#include <stdlib.h>

// we need these for specific things later, I think the names explain good engough for what we will need them
uint32_t heap_beginning;
uint32_t heap_end;
uint32_t last_alloc;
uint32_t memory_used;

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
    }

}