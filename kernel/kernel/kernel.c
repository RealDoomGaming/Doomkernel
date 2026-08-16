#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <interrupts/isr.h>
#include <kernel/tty.h>

extern uint64_t kernel_end;

// in this function we define what happens when we get a breakpoint
void breakpoint_handler(interrupt_frame_t *frame) {
    (void)frame;
    printf("[handler] breakpoint caught, resuming execution\n");
}

// and in this we actually register it
void register_breakpoint_handler() {
    register_interrupt_handler(3, breakpoint_handler);
}
void kernel_main(uint64_t mmap_addr, uint16_t mmap_count) {
    // first thing we do is init the terminal
    terminal_init();
    // just a msg
    printf("[terminal] cursors and color set, buffer set to VGA and screen cleared\n");

    // here we init the entire interrupt stuff
    idt_init();
    pic_remap(0x20, 0x28);
    __asm__ volatile("sti");
    // also just a msg
    printf("[interrupts] IDT loaded, PIC remapped, interrupts enabled\n");

    // for testint purposes we register the handlers
    register_breakpoint_handler();

    // here we make a small test to see if the interrupts work
    printf("[test] triggering breakpoint\n");
    __asm__ volatile("int3");
    printf("[test] we are still alive (no kernel panic)\n");

    // then we init the memory
    printf("[memory] BIOS reported %d usable memory map entries\n", (int64_t)mmap_count);
    // before giving the mmap_addrs to the function we have to convert it
    mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
    memory_init((uint64_t)&kernel_end, mmap, mmap_count);
    // also just a msg
    printf("[memory] heap beginning and end was set\n");

    // printing with our custom printf function :DD
    printf("Successfully booted into the kernel!\n");

    // while loop so the cpu doesnt run off into memory junk
    while (1) {
        __asm__ volatile("hlt");
    }
}