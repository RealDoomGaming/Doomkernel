#include <string.h>

void* memmove(void* destptr, const void* scrptr, size_t size) {
    // here we have to again like in memcpy change the types of the pointers to work with them later
    unsigned char* dest = (unsigned char*) destptr;
    const unsigned char* src = (unsigned char*) srcptr;

    // since memmove is basically the same as memcpy we can do the same
    // but we have to account for overlapping memory and thats why we have the second for loop
    if(dest < src) {
        for (size_t i = 0; i < size; i++) {
            dest[i] = src[i];
        }
    } else {
        for (size_t i = size; i >= 0; i--) {
            dest[i-1] = src[i-1];
        }
    }
}