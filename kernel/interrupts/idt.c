#include <interrupts/idt.h>

// this is the variable to store KERNEL_CS so we dont have to write KERNEL_CS everywhere
static const uint8_t KERNEL_CS = 0x08;
// these are the types for the isrs and irqs
static const uint8_t DEFAULT_TYPE = 0x8E;
static const uint8_t RING_TYPE = 0xEE;

// here will be the actuall code for initing the idt
// and we start with a 256 long array of the idt struct we made before
static interrupter_descriptor_t idt[256];
// but we also need the idtr instruction
static idtr_t idtr;

// this is from the asm file
extern void idt_flush(uint64_t);

// we need this here in order for us to be able to have access to them in the asm file
extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();


// same wit hthe irq handlers
extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

// this is for the vector 0x80 for going out of ring 3
extern void isr128();

// a table for all irs calls
typedef void (*isr_t)(void);
static isr_t isr_table[32] = {
    isr0,  isr1,  isr2,  isr3,
    isr4,  isr5,  isr6,  isr7,
    isr8,  isr9,  isr10, isr11,
    isr12, isr13, isr14, isr15,
    isr16, isr17, isr18, isr19,
    isr20, isr21, isr22, isr23,
    isr24, isr25, isr26, isr27,
    isr28, isr29, isr30, isr31
};

// and a table for all the irq calls
static isr_t irq_table[16] = {
    irq0,  irq1,  irq2,  irq3,
    irq4,  irq5,  irq6,  irq7,
    irq8,  irq9,  irq10, irq11,
    irq12, irq13, irq14, irq15
};

// then for convinience we need a helper which fills one item before we can insert it
static void idt_set_item(int vector, uint64_t handler_addr, uint16_t selector, uint8_t type) {
    // this grabs the lowest 16 bytes so this is exactly the offset we want
    idt[vector].offset_1 = handler_addr & 0xFFFF;
    // this grabs the bist 16-31 and the >> 16 shifts the address right by 16 bits
    idt[vector].offset_2 = (handler_addr >> 16) & 0xFFFF;
    // and this finally grabs bits 32-63 and we shift the address right by 32 bits
    idt[vector].offset_3 = (handler_addr >> 32) & 0xFFFFFFFF;
    // and then we do all the other stuff
    idt[vector].segment_selector = selector;
    idt[vector].ist = 0;
    idt[vector].type = type;
    idt[vector].zero = 0;
}


void idt_set_items() {
    // two for loops for setting the isr and irq stuff
    for (int i = 0; i < 32; i++) {
        idt_set_item(i, (uint64_t)isr_table[i], KERNEL_CS, DEFAULT_TYPE);
    }
    for (int i = 0; i < 16; i++) {
        idt_set_item(i+32, (uint64_t)irq_table[i], KERNEL_CS, DEFAULT_TYPE);
    }

    // this is so we can jump from ring 3 back to ring 0
    idt_set_item(0x80, (uint64_t)isr128, KERNEL_CS, RING_TYPE);
}

void idt_init() {
    // we set up everything for the idtr
    idtr.limit = (sizeof(interrupter_descriptor_t)*256) - 1;
    idtr.base = (uint64_t)&idt;

    // then we call the function which sets everything correctly
    idt_set_items();

    // we need this here to load the table structure
    idt_flush((uint64_t)&idtr);
}

