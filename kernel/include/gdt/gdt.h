#ifndef _GDT_H
#define _GDT_H

// struct for one gdt entry
typedef struct {
    uint16_t limit_low;         // the lower 16 bit of the limit entry in one gdt entry
    uint16_t base_low;          // the lower 16 bit of the base in the entry
    uint8_t base_middle;        // then the next 8 bit of the base
    uint8_t access_flag;        // then we have the access flag which determins in which rings this can be used
    uint8_t granularity;
    uint8_t base_high;          // the last 8 bit of the base
} __attribute__((packed)) gdt_entry;




#endif