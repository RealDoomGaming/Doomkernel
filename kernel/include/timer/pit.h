#ifndef _PIT_H
#define _PIT_H

#include <stdint.h>

extern volatile uint64_t ticks;
extern interrupt_frame_t kernel_frame_template;

void timer_init(uint16_t);

#endif