#ifndef _KERNEL_TTY_H
#define _KERNEL_TTY_H

#include <stddef.h>

void terminal_init();
void terminal_put_char();
void terminal_write();

#endif