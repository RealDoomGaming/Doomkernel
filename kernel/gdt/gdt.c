#include <gdt/gdt.h>
#include <stdint.h>
#include <stdlib.h>

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

void init_gdt() {
    // replace this with a list of gdt entries because right now it isnt a contigous block of memory 
    // like how it should be
    gdt_entry *null_descriptor = malloc(sizeof(gdt_entry));
    gdt_entry *kernel_code_seg = malloc(sizeof(gdt_entry));
    gdt_entry *kernel_data_seg = malloc(sizeof(gdt_entry));
    gdt_entry *user_data_seg = malloc(sizeof(gdt_entry));
    gdt_entry *user_code_seg = malloc(sizeof(gdt_entry));
}