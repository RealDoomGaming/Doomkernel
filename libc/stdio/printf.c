#include <limits.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <panic.h>

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

// this is the function responsible for printing a hex number
// and it returns the amount of characters written or if it fails it returns -1
static int print_hex(uint64_t hex) {
    char buffer[19];        // we have a buffer for 0x and up to 16 numbers and also null
    const char* digits = "0123456789ABCDEF";        // we have all digits which could be in a hex number here

    // this will always be the same since we want to print a hex number
    buffer[0] = '0';
    buffer[1] = 'x';

    // started will be explained later and pos should be self explanetory
    int started = 0;
    int pos = 2;

    // now we have the loop where we walk from the highest nibble (half a byte so 4 bit) down to the lowest
    // and we skip all leading zeros
    for (int i = 60; i > 0; i -= 4) {
        // this would move the nibble we currently care about to the left by i so we can grab it cleanly later
        // and then we AND it with 0xF so that only the position which both have 1 stay 1
        uint8_t nibble = (hex >> i) & 0xF;

        // we only print something if the nibble is not zero so actually worth printing, or 
        // if we already printed a real digit so every nibble matters from now on, or
        // if we have the last digit so even if everything was 0 we would still get 0x0
        if (nibble != 0 || started || i == 0) {
            buffer[pos++] = digits[nibble];
            started = 1;
        }
    }

    // then we set the null terminator
    buffer[pos] = '\0';

    // then we try to print the hex number
    if (!print(buffer, pos)) {
        return -1;
    }

    // and return
    return pos;
}

// this function is used to print decimal numbers
static int print_decimal(int64_t dec) {
    char buffer[21];    // a buffer for printing the sign + up to 19 numbers and then the null terminator
    char temp[20];      // this will hold digits temporary as we get them in reversed order
    int temp_pos = 0;   
    int pos = 0;
    bool negative = false;

    // then we have to handle if the dec is negative or not
    if (dec < 0) {
        negative = true;
        dec = -dec;          // we invert the number here so we make it positive for digit extraction later
    }

    // then we start extracting the digits but from right to left since
    // math operations start extrating them from the rightmost digit
    if (dec == 0) {
        temp[temp_pos++] = '0';
    }
    while (dec > 0) {
        // the modulo isolates the last digit and combing that with '0' converts it to an ASCII
        temp[temp_pos++] = '0' + (dec % 10);
        // and this drops the right most digit because we already added it to temp
        dec /= 10;
    }

    // so now after we got the digits we have them in the wrong order
    // so we need to reverse them
    // and also if the number is negative we have to set the first thing to a minus
    if (negative) {
        buffer[pos++] = '-';
    }

    while (temp_pos > 0) {
        buffer[pos++] = temp[temp_pos--];
    }
    // and of course we have to set the null terminator too
    buffer[pos] = '\0';

    // then we try to print it
    if (!print(buffer, pos)) {
        return -1;
    }

    return pos;
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
                PANIC("Overflow Error");
            }

            // then we need to print or try to print the amount of characters we have now
            if (!print(format, amount)) {
                return -1;
            }

            // then we just add the amount of character we went to the format and also add that to the total written character amount
            format += amount;
            written += amount;

            continue;
        }

        // after we have done all that we can move onto the printing the formating stuff
        
        // we need to save where the format began tho so we can continue from there later
        const char* format_began_at = format++;

        // firstly we see if the user wants to print a character 
        if (*format == 'c') {
            format++;
            // in the arguments a char gets promoted to be an int so we need to convert it back to a char
            char c = (char) va_arg(parameters, int);
            if (!maxrem) {
                // should be an error overflow
                PANIC("Overflow Error");
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
        } else if (*format == 'x') {
            // then if we want to print a hex number
            // this is usefull for adresses and sizes (especially for memory)
            format++;
            const uint64_t hex = va_arg(parameters, const uint64_t);
            int len = print_hex(hex);
            if (len < 0) {
                return -1;
            }
            written += len;
        } else if (*format == 'd') {
            // then if we want to print a decimal number
            format++;
            const int64_t dec = va_arg(parameters, const int64_t);
            int len = print_decimal(dec);
            if (len < 0) {
                return -1;
            }
            written += len;
        } else {
            format = format_began_at;
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