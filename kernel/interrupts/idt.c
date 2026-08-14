#include <interrupts/idt.h>

// here will be the actuall code for initing the idt
// and we start with a 256 long array of the idt struct we made before
static interrupter_descriptor_t idt[256];
// but we also need the idtr instruction
static idtr_t idtr;

// then for convinience we need a helper which fills one item before we can insert it
static void idt_set_item(int vector, uint64_t handler_addr, uint16_t selector, uint8_t type) {
    // this grabs the lowest 16 bytes so this is exactly the offset we want
    idt[vector].offset_1 = handler_addr & 0xFFFF;
    // this grabs the bist 16-31 and the >> 16 shifts the address right by 16 bits
    idt[vector].offset_2 = (handler_addr >> 16) & 0xFFFF;
    // and this finally grabs bits 32-63 and we shift the address right by 32 bits
    idt[vector].offset_3 = (handler_addr >> 32) = 0xFFFFFFFF;
    // and then we do all the other stuff
    idt[vector].segment_selector = selector;
    idt[vector].ist = 0;
    idt[vector].type = type;
    idt[vector].zero = 0;
}


void idt_init() {
    // we set up everything for the idtr
    idtr.limit = (sizeof(interrupter_descriptor_t)*256) - 1;
    idtr.base = (uint64_t)&idt


    // then here we will load an empty table structure
    __asm__ volatile ("lidt %0" : : "m"(idtr));
}