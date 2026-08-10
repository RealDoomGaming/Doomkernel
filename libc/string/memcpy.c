#include <string.h>

void* memcpy(void* destptr, const void* srcptr, size_t size) {
    // destptr and srcptr are well pointers to the original value and thats why we can (and also have to) change the ptrs to a char ptr
    unsigned char* dest = (unsigned char*) destptr;
    const unsigned char* src = (unsigned char*) srcptr;
    
    for (size_t i = 0; i < size; i++) {
        dest[i] = src[i];
    }

    return destptr;
}