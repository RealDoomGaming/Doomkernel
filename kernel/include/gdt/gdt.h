#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

// struct for one gdt entry
typedef struct {
    uint16_t limit_low;         // the lower 16 bit of the limit entry in one gdt entry
    uint16_t base_low;          // the lower 16 bit of the base in the entry
    uint8_t base_middle;        // then the next 8 bit of the base
    uint8_t access_flag;        // then we have the access flag which determins in which rings this can be used
    uint8_t granularity;
    uint8_t base_high;          // the last 8 bit of the base
} __attribute__((packed)) gdt_entry;

// struct for the tss entry
typedef struct {
    uint32_t reserved0;     // these are unused 4 bytes
    // rsp0, 1 and 2 are the privilege Stack pointers for the different rings in the os
    uint64_t rsp0;          
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;     // these are again unused
    uint64_t ist[7];        // this is an array for 7 different distinct stack addresses
    // these next two are also unused bytes
    uint64_t reserved2;     
    uint16_t reserved3;
    uint16_t iomap_base;    // this is an offset for and I/O Permission Bitmap
} __attribute__((packed)) tss_entry;

// struct for the a tss entry in the gdt table
typedef struct {
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access_flag;
    uint8_t granularity;
    uint8_t base_high;
    uint32_t base_upper;
    uint32_t reserved;
} __attribute__((packed)) tss_gdt_entry;

// struct for when we want to load a new gdt with lgdt later
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdt_pointer;

void set_gdt_entry(gdt_entry *entry, uint8_t access, uint8_t flags);
void init_gdt(uint64_t);

#endif