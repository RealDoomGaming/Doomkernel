#include <string.h>

void memset(void* destptr, int value, size_t size) {
    unsigned char* dest = (unsigned char*) destptr;

    for (size_t i = 0; i <= size; i++) {
        dest[i] = (unsigned char) value;
    }
}