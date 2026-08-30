#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <interrupts/isr.h>
#include <kernel/tty.h>
#include <keyboard/keyboard.h>
#include <timer/pit.h>
#include <task/task.h>
#include <fs/fs.h>

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

void kernel_main(uint64_t mmap_addr, uint16_t mmap_count, uint64_t initrd_addr) {
    // first thing we do is init the terminal
    terminal_init();
    // just a msg
    printf("[terminal] cursors and color set, buffer set to VGA and screen cleared\n");

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

    /*
    printf("******TASKS******\n");

    // here we test our task scheduler by firstly making two tasks
    printf("[tasks] created two test tasks which print A and B\n");
    task_create(task_a);
    task_create(task_b);
    task_create(task_reaper);
    scheduler_enable();*/

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

    // printing with our custom printf function :DD
    printf("Successfully booted into the kernel!\n");

    // while loop so the cpu doesnt run off into memory junk
    while (1) {
        __asm__ volatile("hlt");
    }
}