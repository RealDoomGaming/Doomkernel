#include <gdt/gdt.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// in order for the new gdt to be abe to get loaded it needs to be a list of gdt structs
// (Null + kernel code and data + user code and data + tss (takes 2 since its 16bytes))
gdt_entry gdt[7] __attribute__((aligned(16)));

// the global tss entry
tss_entry system_tss;

// the gdt pointer we need to be able to load the gdt
gdt_pointer gdtp;

// the two extern asm functions
extern void load_gdt(gdt_pointer *ptr, uint16_t code_sel, uint16_t data_sel);
extern void load_tss(uint16_t tss_sel);

void set_gdt_entry(gdt_entry *entry, uint8_t access, uint8_t flags) {
    entry->limit_low = 0xFFFF; // this doesnt really matter in long mode since it gets ignored
    entry->base_low = 0x00; // the base is also ignored 
    entry->base_middle = 0x0000;
    entry->access_flag = access;
    // with granularity we have 1 byte with 2 different things
    // we want to keep the left 4 bit so we and the flags wit 0xF0 which is 11110000
    // and then after that we set the right 4 bit to 1111 so the cpu knows that the 
    // memory size is supposed to be the max possible size (we do this with the OR and 0x0F)
    entry->granularity = (flags & 0xF0) | 0x0F; 
    entry->base_high = 0x00;
}

void setup_tss_descriptor(tss_gdt_entry *tss_desc, uint64_t tss_base, uint32_t tss_limit) {
    // here we basically just split up the base into their own individual bits
    // we do this by ANDing everyting with all 1 bits so we keep the stuff we want
    tss_desc->base_low = (uint16_t) tss_base & 0xFFFF;
    tss_desc->base_middle = (uint8_t) (tss_base >> 16) & 0xFF;
    tss_desc->base_high = (uint8_t) (tss_base >> 24) & 0xFF;
    tss_desc->base_upper = (uint32_t) (tss_base >> 32);

    // then we set the segment limits (granularity and limit)
    tss_desc->limit_low = (uint16_t) tss_limit & 0xFFFF;        // here we get the bits from 0 to 5
    tss_desc->granularity = (uint8_t) (tss_limit >> 16) & 0xF;  // and here from 16 to 19

    // and then we set the access flag
    // Bits 0 to 3 are type 64bit tss available (1001 = 0x9)
    // Bit 4 is the System Descriptor (0)
    // Bits 5 to 6 are the dpl ring (00)
    // and Bit 7 is the Present flag (1)
    tss_desc->access_flag = 0x89;

    // and the last thing is if its a reserved fiel
    tss_desc->reserved = 0;
}

void init_gdt(uint64_t kernel_top) {
    // firstly zero out the area where the tss entry will live
    memset(&system_tss, 0, sizeof(tss_entry));
    system_tss.rsp0 = kernel_top;   // we set the rsp0 pointer to the top of the kernel
    system_tss.iomap_base = sizeof(tss_entry); // and here we disable the io mapping

    // then we need to init the entire gdt array
    // this is just the null descriptor so we can zero everything out
    memset(&gdt[0], 0, sizeof(gdt_entry));
    // then this is the kernel code segment
    // the access flag is 0x9A because the segment should be present, in rint 0, a segment, executable and read/writeable
    // and the other flag is 0xA0 because we are in long mode and we wan granularity
    set_gdt_entry(&gdt[1], 0x9A, 0xA0);    
    // then we have the kernel data sement
    // the access flag is 0x92 because the segment should be present, in ring 0, a segment and only read/writeable and not executable
    // the other flag is 0x00 because we dont need to set it for 64 bit
    set_gdt_entry(&gdt[2], 0x92, 0x00);    
    // then we have the user data segment
    // the access flag is 0xF2 because it should be present, in ring 3, a segment and read/writeable
    set_gdt_entry(&gdt[3], 0xF2, 0x00);       
    // then we have the user code segment
    // the access flag is 0xFA because it should be present, in ring 3, a segment, executable and read and writeable
    set_gdt_entry(&gdt[4], 0xFA, 0xA0);    

    // and then for our 5th and 6th entry we have the tss
    // but for the tss we need to make a new tss gdt entry struct
    tss_gdt_entry *tss_desc = (tss_gdt_entry*)&gdt[5];
    // then we define the tss base and the tss limit
    uint64_t tss_base = (uint64_t)&system_tss;
    uint32_t tss_limit = sizeof(system_tss) - 1;    // the limit of the entire tss is the size - 1 byte

    setup_tss_descriptor(tss_desc, tss_base, tss_limit);

    // lastly we actually need to load it
    // so before loading it we need to set the pointer of where the gdt is
    gdtp.limit = sizeof(gdt)-1;
    gdtp.base = (uint64_t)&gdt;

    // then after setting the pointer we need to call the asm function for loading the gdt
    // here we pass the pointer to the gdt pointer struct
    // then we also pass 0x08 which means the kernel code selector since the first sector of the gdt is null
    // and then finally we ass 0x10 which means the kernel data selector and 0x10 offsets it by 16 byte so it can go to the third segement the kernel data segment
    load_gdt(&gdtp, 0x08, 0x10);

    // and then after that we need to call the asm function for loading the tss
    // here we only pass one thing and thats the corresponding number to index 5 in the gdt so it goes to that index
    load_tss(0x28);
}