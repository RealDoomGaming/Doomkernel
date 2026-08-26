#ifndef _PIT_H
#define _PIT_H

#include <stdint.h>

extern volatile uint64_t ticks;

void timer_init(uint16_t);

#endif