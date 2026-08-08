#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

void terminal_init();
void terminal_put_char(char c);
void terminal_write(const char* data, size_t length);

#endif