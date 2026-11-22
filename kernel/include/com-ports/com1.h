#ifndef __COM1_H
#define __COM1_H

#include <stdint.h>

// what the port of the com1 port is
#define COM1_PORT 0x3f8

uint8_t init_com1();
void write_com1(uint8_t);
void write_string(const char*)

#endif