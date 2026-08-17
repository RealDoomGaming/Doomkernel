#include <stdlib.h>
#include <string.h>
#include <limits.h>

void* realloc(void* ptr, size_t size) {
    // realloc will make a new memory chunck, and shift everything of the old data over

    // if we dont get a valid pointer we cant do anything
    if (!ptr) {
        return 0;
    }
    
    // firstly check if the size if 0
    if (size == 0) {
        free(ptr);
        return 0;
    }

    // then we do the same check as in malloc, we need to check if our newly aligned size is bigger then the integer limit
    if (size > INT_MAX - (ALLOC_ALIGN - 1)) {
        return 0;
    }
    // and if it isnt we can safely align it
    size = ALIGN_UP(size);

    // we need to find the alloc_t struct which is infront of the pointer
    alloc_t *alloc_cur = (alloc_t *)((uint8_t*)ptr - sizeof(alloc_t));

    // then we want to see if the old size is bigger then the new size
    if (alloc_cur->size >= size) {
        // if we want to shrink the memory we firstly have to see if it is even worth it because we have
        // to split the memory chunck into 2

        if (alloc_cur->size - size > sizeof(alloc_t)) {
            // make a new memory chunck here
            alloc_t *remainder = (alloc_t *)((uint8_t *)alloc_cur + sizeof(alloc_t) + size);
            // then we assign it everything
            remainder->status = 0;
            remainder->size = alloc_cur->size - size - sizeof(alloc_t);

            // then we set the size of the memory chunck which we wanted to reduce to the reduced size
            alloc_cur->size = size;
        }


        // and return
        return ptr;
    }

    // then we easily get the new bigger memory chunck with malloc
    void *new = malloc(size);
    if (!new) {
        return 0;
    }

    // then we can memcpy the data over
    memcpy(new, ptr, alloc_cur->size);
    // and finally we free the old pointer
    free (ptr);

    return new;
}