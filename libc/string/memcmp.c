#include <string.h>

int memcmp(const char* string1, const char* string2, size_t s1_len) {
    // this function compares string 1 and string 2 up to the length of string 1
    // if string 1 is greater then we return 1
    // else if string 2 is greater then we return -1 
    // and if they are equal we return 0

    // we need to make the chars to unsigned chars because comparing signed chars can get funky
    const unsigned char* s1 = (unsigned char*) string1;
    const unsigned char* s2 = (unsigned char*) string2;

    for (size_t i = 0; i < s1_len; i++) {
        if (s1[i] > s2[i]) {
            return 1;
        } else if (s1[i] < s2[i]) {
            return -1;
        }
    }

    return 0;
}