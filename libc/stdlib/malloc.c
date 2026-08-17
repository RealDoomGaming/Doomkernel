#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// this is the maximum that we can map because of the entries we made in the bootloader
#define MAPPED_LIMIT 0x40000000UL

uint64_t heap_beginning = 0;;
uint64_t heap_end = 0;
uint64_t last_alloc = 0;
uint64_t memory_used = 0;;


void memory_init(uint64_t kernel_end, mmap_entry_t *mmap, uint16_t mmap_count) {
    heap_beginning = kernel_end + 0x1000;   // the beginning of the heap is the kernel end with a bit of a buffer between them

    // debugging messages
    printf("[memory] kernel end at %x\n", kernel_end);
    printf("[memory] heap beginning padded to %x\n", heap_beginning);

    // this will be used to track the biggest continuous usable chunck of memory
    uint64_t best_base = 0;
    uint64_t best_len = 0;

    // this is the for loop for actually finding the biggest continuous usable chunck of memory
    for (uint16_t i = 0; i < mmap_count; i++) {
        // getting the current entry
        mmap_entry_t *entry = &mmap[i];

        // if we cannot use this current entry we just go to the next entry
        // so this basically filters out reserved or unusable regions
        if (entry->type != MMAP_USABLE) {
            continue;
        }

        // here we get where the entry starts and where it ends
        uint64_t entry_start = entry->base_addr;
        uint64_t entry_end = entry_start + entry->length;

        // then we check if the entry is below our defined heap beginning so this prevents the heap from placing itself in low ram
        if (entry_end <= heap_beginning) {
            continue;
        }

        // then if an entry starts before our heap beginning but ends after our heap beginning we just shift its starting addr
        if (entry_start < heap_beginning) {
            entry_start = heap_beginning;
        }

        // and because of that we have to calculate the length new here even if the entry doesnt start below our defined heap beginning
        uint64_t new_length = entry_end - entry_start;

        // debugging message
        printf("[memory] found usable region with length %x\n", new_length);

        // then we see if this is the biggest entry yet
        if (new_length > best_len) {
            best_len = new_length;
            best_base = entry_start;
        }
    }

    // and then finally after searching for the biggest entry we have a fallback
    if (best_len == 0) {
        heap_end = heap_beginning + 0x100000;
    } else {
        heap_beginning = best_base;
        heap_end = best_base + best_len;
    }

    if (heap_end > MAPPED_LIMIT) {
        heap_end = MAPPED_LIMIT;
    }

    last_alloc = heap_beginning;
    memory_used = 0;

    // some messages for error printing
    printf("[memory] heap set to beginning=%x and end=%x\n", heap_beginning, heap_end);
}

void* malloc(size_t size) {
    // in this function we will just allocate memory for the use by using the struct we defined in the header file to indicate where allocated memory starts
    
    // if the size is 0 or lower then we can just return as we wont allocate anything
    if (size <= 0) {
        return 0;
    }

    if (heap_beginning == 0) {
        // not inited yet
        return 0;
    }

    // before we try to align anything we need to if adding (ALLOC_ALIGN - 1) to size will exceed the max amount an int can hold
    if (size > MAX_SIZE - (ALLOC_ALIGN - 1)) {
        return 0;
    }
    // if we pass that check we can align our size without any risks
    size = ALIGN_UP(size);

    // then else if we actually have a size to allocate
    // we need to know where our heap began
    uint8_t *memory_loc = (uint8_t*) heap_beginning;
    // and then loop through our existing blocks so if we find a free one which has the required size we can give it back
    while ((uint64_t) memory_loc < last_alloc) {
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
        if (alloc->size >= (uint64_t) size) {
            // if the size fits for us we can actually use this memory location

            // if the memor which is leftover is big enough for another head plus atleast 1 byte we make a whole new block
            if (alloc->size - size > sizeof(alloc_t)) {
                // we get the new alloc which is left over now and set its status and size
                alloc_t *remainder = (alloc_t*)((uint8_t*)alloc + sizeof(alloc_t) + size);
                remainder->status = 0;
                remainder->size = alloc->size - size - sizeof(alloc_t);
                // but we also have to cut down the size of our current memory chunck alloc
                alloc->size = size;
            }

            // firstly we set the current structs availability to 1 so its marked as used
            alloc->status = 1;
            memory_used += alloc->size + sizeof(alloc_t);
            // clear the memory when we reuse an old memory block
            memset((void*)((uint64_t)alloc + sizeof(alloc_t)), 0, alloc->size);
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
        return 0;
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
    memset((void*)((uint64_t)new + sizeof(alloc_t)), 0, size);

    // and then we just return a pointer to that free memory
    return (void*)((uint64_t)new + sizeof(alloc_t));
}