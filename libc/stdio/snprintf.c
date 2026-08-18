#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool write_to_buffer(const char* data, size_t length, char* buffer, int written) {
    const unsigned char* bytes = (const unsigned char*) data;

    for (size_t i = 0; i < length; i++) {
        buffer[i+written] = bytes[i];
    }

    return true;
}

int snprintf(char* buffer, size_t value, const char* format, ...) {
    // this function just basically does the same as print, but instead of writing them 
    // it puts the characters into the buffer for how many times was defined in the value parameter
    // this is the same code as in the printf function just modified a bit so for comments refer to the printf.c file

    va_list parameters;
    va_start(parameters, format);

    if (value == 0) {
        return 0;
    }

    size_t limit = value - 1;
    int written = 0;

    while (*format != '\0' && (size_t)written < limit) {

        size_t maxrem = limit - written;

        if (format[0] != '%' || format[1] == '%') {
            if (format[0] == '%') {
                format++;
            }

            size_t amount = 1;
            while (format[amount] && format[amount] != '%' && amount < limit) {
                amount++;
            }

            if (maxrem < amount) {
                return -1;
            }

            if (!write_to_buffer(format, amount, buffer, written)) {
                return -1;
            }

            format += amount;
            written += amount;

            continue;
        }

        const char* format_began_at = format++;

        if (*format == 'c') {
            format++;
            char c = (char) va_arg(parameters, int);
            if (!maxrem) {
                return -1;
            }

            if(!write_to_buffer(&c, sizeof(c), buffer, written)) {
                return -1;
            }

            written++;
        } else if (*format == 's') {

            format++;

            const char* str = va_arg(parameters, const char*);
            size_t len = strlen(str); 
            if (maxrem < len) {
                return -1;
            }

            if (!write_to_buffer(str, len,  buffer, written)) {
                return -1;
            }

            written += len;
        } else {
            format = format_began_at;
            size_t len = strlen(format);
            if (maxrem < len) {
                return -1;
            }
            if (!write_to_buffer(format, len,  buffer, written)) {
                return -1;
            }
            written += len;
            format += len;
        }
    }

    va_end(parameters);
    buffer[written] = '\0';
    return written;
}