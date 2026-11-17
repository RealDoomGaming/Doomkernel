#include <gdt/gdt.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// this is for the gdt entries which will be needed
static gdt_entry gdt[7] __attribute__((aligned(16)));
// then we need one tss entry defined here, we only need one because the system also only needs one
static tss_entry system_tss;
// pointer to the gdt
static gdt_pointer gdtp;

// then we only need 2 asm declarations for loading the gdt and the tss
extern void load_gdt(gdt_pointer *ptr, uint16_t code_sel, uint16_t data_sel);
extern void load_tss(uint16_t tss_sel);

/*
This is a table of the access and flags for the gdt entries
Slot    Hex    Access       Flags
Kernel code    0x9A         0x0A 
Kernel data    0x92         0x00
User data    0xF2           0x00
User code    0xFA           0x0A
TSS    0x89    1000 1001    none
*/
// then we make a table where all of the access flags are defined
uint8_t gdt_access_flags[5] = {0x9A, 0x92, 0xF2, 0xFA, 0x89};
// and make 2 constant variables for the flags
const uint8_t gdt_flags_code = 0xA; // for code segments
const uint8_t gdt_flags_data = 0x0; // for data segments

void set_gdt_entry(gdt_entry *entry, uint8_t access, uint8_t flags) {
    // since we only zeroed the gdt entry 0 before we need to zero the current entry too before we do anything
    memset(entry, 0, sizeof(gdt_entry));

    entry->limit_low = 0xFFFF;  // these are all ones so 16 ones
    entry->base_low = 0x00;
    entry->base_middle = 0x00;
    entry->access_flag = access;    // we just copy the access flags from what the function gets
    entry->granularity = ((flags & 0xF) << 4) | 0xF; // the 0xF is four ones
    entry->base_high = 0x00;
}

/*
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
*/
void build_tss_descriptor(tss_gdt_entry *tss_desc, uint64_t tss_base, uint32_t tss_limit) {
    // firstly zero the tss_desc
    memset(tss_desc, 0, sizeof(tss_gdt_entry));

    // then we fill the fields
    tss_desc->limit_low = tss_limit & 0xFFFF;
    tss_desc->base_low = tss_base & 0xFFFF;
    tss_desc->base_middle = (tss_base >> 16) & 0xFF;
    tss_desc->base_high = (tss_base >> 24) & 0xFF;
    tss_desc->base_upper = (tss_base >> 32) & 0xFFFFFFFF;
    tss_desc->access_flag = 0x89;
    tss_desc->granularity = (tss_limit >> 16) & 0xF;
    tss_desc->reserved = 0;
}

void init_gdt(uint64_t kernel_top) {
    // before doing anything at all we need to zero out the location where the tss entry and the gdt entries will be
    memset(&system_tss, 0, sizeof(tss_entry));
    memset(&gdt[0], 0, sizeof(gdt_entry));

    // in this loop we will set all of the gdt entries
    uint8_t right_flags = 0;
    for (int i = 1; i < 6; i++) {
        if (i == 1 || i == 4) {
           right_flags = gdt_flags_code;
        } else {
            right_flags = gdt_flags_data;
        }

        set_gdt_entry(&gdt[i], gdt_access_flags[i-1], right_flags);
    }

    // we need to set the rsp0 from the tss entry to the kernel stack top
    system_tss.rsp0 = kernel_top;
    // then we also need to set the iomap of the tss to the size of the tss entry
    system_tss.iomap_base = sizeof(tss_entry);

    // then we need to build the tss descriptor which will live inside the gdt
    tss_gdt_entry *gdt_desc = (tss_gdt_entry*)&gdt[5];
    uint64_t tss_base = (uint64_t)&system_tss;
    uint32_t tss_limit = sizeof(system_tss) - 1; 

    // we build the tss gdt descriptor here
    build_tss_descriptor(gdt_desc, tss_base, tss_limit);

    // here we build the gdt pointer
    gdtp.limit = sizeof(gdt)-1;
    gdtp.base = (uint64_t)&gdt;

    // and then we load the gdt
    load_gdt(&gdtp, 0x08, 0x10);

    // after that we can load the tss
    load_tss(0x28);
}