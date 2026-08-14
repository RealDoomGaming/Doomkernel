#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

// this will be each item of the interrupter descriptor table (idt)
typedef struct {
    uint16_t offset_1;          // this is the first offset in the item
    uint16_t segment_selector;  // this is the segment selector in the item (must point to a valid code item in the gdt)
    uint8_t ist;                // holds the Interrupter Stack table offset 
    uint8_t type;               // this holds the gate type, dpl and p fields in the item
    uint16_t offset_2;          // second offset in the item
    uint32_t offset_3;          // third offset in the item
    uint32_t zero;              // reserved bits in the item
} interrupter_descriptor;

void idt_init();

#endif