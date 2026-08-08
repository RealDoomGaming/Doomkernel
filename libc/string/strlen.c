#include <string.h>

// pretty simple should be easily understandable
size_t strlen(const char* str) {
    size_t len = 0;
    // you could also just check for str[len] but I like it more like this
    while (str[len] != '\0') {
        len++;
    }

    return len;
}