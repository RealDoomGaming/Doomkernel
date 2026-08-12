#include <stdlib.h>
#include <string.h>

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

    // we need to find the alloc_t struct which is infront of the pointer
    alloc_t *alloc_cur = (alloc_t *)((uint8_t*)ptr - sizeof(alloc_t));

    // then we want to see if the old size is bigger then the new size
    if (alloc_cur->size >= size) {
        // here we could actually shrink the memory but this is the lazy approach
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