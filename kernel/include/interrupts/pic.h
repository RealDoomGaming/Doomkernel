#ifndef _PIC_H
#define _PIC_H

#include <stdint.h>

// PIC stands for Programmable interface controller and we manage hardware interrupts with it
// we need a few functions
void pic_remap(int, int); // pic remap is needed to reconfigure the master and slave 8259 chips to send custom interupt signals
void pic_send_eoi(uint8_t);             // this send and eoi (End of interupt) signal back to the PIC chips
void pic_set_mask(uint8_t);             // this disables (or masks however you wanna call it) a specific IRQ
void pic_clear_mask(uint8_t);            // and this enables (unmasks) a specific IRQ


#endif