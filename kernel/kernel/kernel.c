#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "user_code.h"
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <interrupts/isr.h>
#include <kernel/tty.h>
#include <keyboard/keyboard.h>
#include <timer/pit.h>
#include <task/task.h>
#include <fs/fs.h>
#include <gdt/gdt.h>

extern uint64_t kernel_end;
extern uint8_t kernel_stack_top;

// everything here is for user mode stuff
extern void enter_user_mode(uint64_t entry_point, uint64_t user_stack, uint16_t user_cs, uint16_t user_ds);
extern uint8_t __user_text_start[];
extern uint8_t __user_text_end[];

#define USER_CODE_BASE 0x400000ULL
#define USER_STACK_TOP 0x800000ULL


// in this function we define what happens when we get a breakpoint
void breakpoint_handler(interrupt_frame_t *frame) {
    (void)frame;
    printf("[handler] breakpoint caught, resuming execution\n");
}

// and in this we actually register it
void register_breakpoint_handler() {
    register_interrupt_handler(3, breakpoint_handler);
}

// these two functions are for testing the scheduler later
void task_a() {
    for (int i = 0; i < 20; i++) {
        printf("A");
    }

    task_exit();
}
void task_b() {
    for (int i = 0; i < 20; i++) {
        printf("B");
    }

    task_exit();
}

static inline uint64_t read_cr3(void)
{
    uint64_t v;
    __asm__ volatile("mov %%cr3, %0" : "=r"(v));
    return v;
}

static void print_entry(const char *name, uint64_t e)
{
    printf("%s: %x_%x\n", name, (uint32_t)(e >> 32), (uint32_t)(e & 0xFFFFFFFF));
}

void dump_paging(void)
{
    uint64_t cr3 = read_cr3();

    uint64_t *pml4 = (uint64_t *)(cr3 & 0x000FFFFFFFFFF000ULL);
    uint64_t *pdpt = (uint64_t *)(pml4[0] & 0x000FFFFFFFFFF000ULL);
    uint64_t *pdt  = (uint64_t *)(pdpt[0] & 0x000FFFFFFFFFF000ULL);

    print_entry("CR3   ", cr3);
    print_entry("PML4[0]", pml4[0]);
    print_entry("PDPT[0]", pdpt[0]);
    print_entry("PDT[1] ", pdt[1]);
    print_entry("PDT[2] ", pdt[2]);
    print_entry("PDT[3] ", pdt[3]);
}

void kernel_main(uint64_t mmap_addr, uint16_t mmap_count, uint64_t initrd_addr) {
    // first thing we do is init the terminal
    terminal_init();
    // just a msg
    printf("[terminal] cursors and color set, buffer set to VGA and screen cleared\n");

    // we init the gdt and tss here
    printf("******GDT & TSS******\n");

    // here we get the kernel stack top
    uint64_t stack_top_addr = (uint64_t)&kernel_stack_top;
    // and then we init the gdt and tss stuff
    init_gdt(stack_top_addr);
    printf("[gdt & tss] GDT and TSS successfully loaded\n");

    printf("******INTERRUPTS******\n");

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

    printf("******KEYBOARD******\n");

    // we need to init the keyboard here
    keyboard_init();
    printf("[keyboard] irq1 registered\n");
    // then we also have a test where we stop everything and have the user type something and escape is for exiting this loop
    printf("[test] type something and press escape to stop\n");
    char typed;
    do {
        // we get a input key
        typed = keyboard_get_key();
        // and we directly print it to the terminal
        terminal_put_char(typed);
    } while (typed != 27); // 27 stands for the escape key
    printf("\n");

    printf("******TIMER******\n");

    // we init the timer here with 100hz
    timer_init(100);
    printf("[timer] timer pit initialized with 100hz\n");

    // then we wait for 300 ticks (3 seconds) so we know irq0 is active and firing
    uint64_t start = ticks;
    while (ticks - start < 300) {
        __asm__ volatile("hlt");
    }
    printf("[timer] 300 ticks passed (3 seconds) so the timer is alive!\n");

    printf("******MEMORY******\n");

    // then we init the memory
    printf("[memory] BIOS reported %d usable memory map entries\n", (int64_t)mmap_count);
    // before giving the mmap_addrs to the function we have to convert it
    mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
    memory_init((uint64_t)&kernel_end, mmap, mmap_count);
    // also just a msg
    printf("[memory] heap beginning and end was set\n");

    printf("******FILESYSTEM******\n");

    // we init the filesystem here
    fs_init(initrd_addr);
    // then for testing we try to get our file we read when compiling
    char query[32] = "doom.txt";
    initrd_entry_t *entry = fs_read(query);

    // and then we check if we found our entry
    if (entry) {
        printf("[fs] %s with size = %x\n", entry->name, entry->size);

        char *data = (char *)fs_get_data(entry);

        // we do this here because we dont know if the file is null terminated
        for (uint32_t i = 0; i < entry->size; i++) {
            terminal_put_char(data[i]);
        }

        printf("\n");
    } else {
        printf("[fs] %s file not found\n", query);
    }

    printf("******TASKS******\n");

    // here we test our task scheduler by firstly making two tasks
    printf("[tasks] created two test tasks which print A and B\n");
    /*task_create(task_a);
    task_create(task_b);
    task_create(task_reaper);*/
    scheduler_enable();

    printf("******USER MODE******\n");
    // here we will test our user mode by entering it then testing something and then returning to ring 0
    // but firstly we need to define some stuff
    uint16_t user_cs = 0x23;     // this is for the gdt user code segement
    uint16_t user_ds = 0x1B;     // and this is for the gdt user data segment
    
    uint64_t size = __user_text_end - __user_text_start;    // we get the entire size of the user text
    uint64_t offset = (uint64_t)user_test_entry - (uint64_t)__user_text_start;  // then we get the offset of the distance between the start of our user text and the test function

    memcpy((void *)USER_CODE_BASE, __user_text_start, size);    // then we copy the memory from the start to end of the user text into the User code base

    dump_paging();

    printf("[user mode] jumping into user space\n");
    enter_user_mode(USER_CODE_BASE + offset, USER_STACK_TOP, user_cs, user_ds); // and try to enter user mode

    // printing with our custom printf function :DD
    printf("Successfully booted into the kernel!\n");

    // while loop so the cpu doesnt run off into memory junk
    while (1) {
        __asm__ volatile("hlt");
    }
}