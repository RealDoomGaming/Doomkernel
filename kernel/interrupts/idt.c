#include <interrupts/idt.h>

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
    idt_set_item(0,  (uint64_t)isr0,  0x18, 0x8E);
    idt_set_item(1,  (uint64_t)isr1,  0x18, 0x8E);
    idt_set_item(2,  (uint64_t)isr2,  0x18, 0x8E);
    idt_set_item(3,  (uint64_t)isr3,  0x18, 0x8E);
    idt_set_item(4,  (uint64_t)isr4,  0x18, 0x8E);
    idt_set_item(5,  (uint64_t)isr5,  0x18, 0x8E);
    idt_set_item(6,  (uint64_t)isr6,  0x18, 0x8E);
    idt_set_item(7,  (uint64_t)isr7,  0x18, 0x8E);
    idt_set_item(8,  (uint64_t)isr8,  0x18, 0x8E);
    idt_set_item(9,  (uint64_t)isr9,  0x18, 0x8E);
    idt_set_item(10, (uint64_t)isr10, 0x18, 0x8E);
    idt_set_item(11, (uint64_t)isr11, 0x18, 0x8E);
    idt_set_item(12, (uint64_t)isr12, 0x18, 0x8E);
    idt_set_item(13, (uint64_t)isr13, 0x18, 0x8E);
    idt_set_item(14, (uint64_t)isr14, 0x18, 0x8E);
    idt_set_item(15, (uint64_t)isr15, 0x18, 0x8E);
    idt_set_item(16, (uint64_t)isr16, 0x18, 0x8E);
    idt_set_item(17, (uint64_t)isr17, 0x18, 0x8E);
    idt_set_item(18, (uint64_t)isr18, 0x18, 0x8E);
    idt_set_item(19, (uint64_t)isr19, 0x18, 0x8E);
    idt_set_item(20, (uint64_t)isr20, 0x18, 0x8E);
    idt_set_item(21, (uint64_t)isr21, 0x18, 0x8E);
    idt_set_item(22, (uint64_t)isr22, 0x18, 0x8E);
    idt_set_item(23, (uint64_t)isr23, 0x18, 0x8E);
    idt_set_item(24, (uint64_t)isr24, 0x18, 0x8E);
    idt_set_item(25, (uint64_t)isr25, 0x18, 0x8E);
    idt_set_item(26, (uint64_t)isr26, 0x18, 0x8E);
    idt_set_item(27, (uint64_t)isr27, 0x18, 0x8E);
    idt_set_item(28, (uint64_t)isr28, 0x18, 0x8E);
    idt_set_item(29, (uint64_t)isr29, 0x18, 0x8E);
    idt_set_item(30, (uint64_t)isr30, 0x18, 0x8E);
    idt_set_item(31, (uint64_t)isr31, 0x18, 0x8E);

    idt_set_item(32, (uint64_t)irq0,  0x18, 0x8E);
    idt_set_item(33, (uint64_t)irq1,  0x18, 0x8E);
    idt_set_item(34, (uint64_t)irq2,  0x18, 0x8E);
    idt_set_item(35, (uint64_t)irq3,  0x18, 0x8E);
    idt_set_item(36, (uint64_t)irq4,  0x18, 0x8E);
    idt_set_item(37, (uint64_t)irq5,  0x18, 0x8E);
    idt_set_item(38, (uint64_t)irq6,  0x18, 0x8E);
    idt_set_item(39, (uint64_t)irq7,  0x18, 0x8E);
    idt_set_item(40, (uint64_t)irq8,  0x18, 0x8E);
    idt_set_item(41, (uint64_t)irq9,  0x18, 0x8E);
    idt_set_item(42, (uint64_t)irq10, 0x18, 0x8E);
    idt_set_item(43, (uint64_t)irq11, 0x18, 0x8E);
    idt_set_item(44, (uint64_t)irq12, 0x18, 0x8E);
    idt_set_item(45, (uint64_t)irq13, 0x18, 0x8E);
    idt_set_item(46, (uint64_t)irq14, 0x18, 0x8E);
    idt_set_item(47, (uint64_t)irq15, 0x18, 0x8E);
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