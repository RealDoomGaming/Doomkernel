#include <gdt/gdt.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
/*
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
}*/

// this is for the gdt entries which will be needed
static gdt_entry gdt[7] __attribute__((aligned(16)));
// then we need one tss entry defined here, we only need one because the system also only needs one
static tss_entry system_tss;

// then we only need 2 asm declarations for loading the gdt and the tss
extern void load_gdt();
extern void load_tss();

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
    memset(entry, 0, sizeof(entry));

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
    tss_desc->limit_low = tss_limit & 0xFF;
    tss_desc->base_low = tss_base & 0xFF;
    tss_desc->base_middle = (tss_base << 16) & 0xF;
    tss_desc->base_high = (tss_base << 24) & 0xF;
    tss_desc->base_upper = (tss_base << 32) & 0xFFFF;
    tss_desc->access_flag = 0x89;
    tss_desc->granularity = ;
    tss_desc->reserved = 0;
}

void init_gdt(uint64_t) {
    // before doing anything at all we need to zero out the location where the tss entry and the gdt entries will be
    memset(&system_tss, 0, sizeof(tss_entry));
    memset(&gdt[0], 0, sizeof(gdt_entry));

    // in this loop we will set all of the gdt entries
    uint8_t right_flags = 0;
    for (int i = 1; i < 6; i++) {
        if (i == 1 || i == 5) {
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
    tss_gdt_entry *gdt_desc = (tss_gdt_entry)&gdt[5];
    uint64_t tss_base = (uint64_t)&system_tss;
    uint32_t tss_limit = sizeof(system_tss) - 1; 

    build_tss_descriptor(gdt_desc, tss_base, tss_limit);
}