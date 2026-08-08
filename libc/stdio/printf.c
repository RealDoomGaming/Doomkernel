#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

// this is the function for printing the amount of characters between each %
static bool print(const char* data, size_t lenght) {
    // we convert the data so its easier to use later
    const unsigned char* bytes = (const unsigned char*) data;

    for (size_t i = 0; i < lenght; i++) {
        // then we try to put the character to the output and if we get an EOF back that failed
        if (putchar(bytes[i]) == EOF) {
            return false;
        }
    }

    return true;
}

// we need our printf function here since this is the printf file
int printf(const char* format, ...) {
    // in this function we need to firstly get a list of all arguments passed
    // after that we need to start our while loop which runs until we hit a \0
    // in the while loop we firstly want to find the plain text in format from our current position in format to the next %
    // we then want to print that text we found and only after that we want to print the formatted stuff
    // and then repeat that loop until we hit \0

    va_list parameters;
    va_start(parameters, format);

    // how much we have already written
    int written = 0;

    while (*format != '\0') {

        size_t maxrem = INT_MAX - written;

        // if the first thing in the string we should print isnt a % and the second is one we go into here else we skip down to printing the formatted stuff
        if (format[0] != '%' || format[1] == '%') {
            // then if the user wanted to print a literall % then the first and second characters would need to be % thats why we check here
            if (format[0] == '%') {
                format++;
            }

            // the amount of how much we needed to move, we will use this later on
            size_t amount = 1;
            // then while another character in the format exists and that one isnt a % we add 1 to amount until we hit the end of hit a % 
            while (format[amount] && format[amount] != '%') {
                amount++;
            }

            // if the amount we need to print is bigger an the max int we will need to give back an overflow error
            if (maxrem < amount) {
                // what I still need to do is have a error variable somewhere so I can set that to the error
                return -1;
            }

            // then we need to print or try to print the amount of characters we have now
            if (!print(format, amount)) {
                return -1;
            }

            // then we just add the amount of character we went to the format and also add that to the total written character amount
            format += amount;
            written += amount;
        }

        // after we have done all that we can move onto the printing the formating stuff
        
        // we need to save where the format began tho so we can continue from there later
        const char* format_began_at = format++

        // firstly we see if the user wants to print a character 
        if (*format == 'c') {
            format++;
            // in the arguments a char gets promoted to be an int so we need to convert it back to a char
            char c = (char) va_arg(parameters, int);
            if (!maxrem) {
                // should be an error overflow
                return -1;
            }
            // then we just print that one character we wanted to print via the formatting
            if(!print(&c, sizeof(c))) {
                return -1;
            }
            // and add one to the total written characters
            written++;
        } else if (*format == 's') {
            // then else if we wanted to print a string
            format++;
            // we get that parameter
            const char* str = va_arg(parameters, const char*);
            size_t len = strlen(str); 
            if (maxrem < len) {
                // error overflow
                return -1;
            }
            // then try to print the string
            if (!print(str, len)) {
                return -1;
            }
            // then add the length of the string to the total amounts of characters written
            written += len;
        } else {
            format = format_began_at
            size_t len = strlen(format);
            if (maxrem < len) {
                // error overflow
                return -1;
            }
            // try print again
            if (!print(format, len)) {
                return -1;
            }
            written += len;
            format += len;
        }
    }

    // then we have to end the parameter reading thing
    va_end(parameters);
    // and we return the total amount of character written
    return written;
}