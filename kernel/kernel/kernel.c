#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <globals.h>

#include "user_code.h"
#include <syscalls/syscall.h>
#include <interrupts/idt.h>
#include <interrupts/pic.h>
#include <interrupts/isr.h>
#include <kernel/tty.h>
#include <keyboard/keyboard.h>
#include <timer/pit.h>
#include <task/task.h>
#include <fs/fs.h>
#include <gdt/gdt.h>
#include <com-ports/com1.h>
#include <video/vbe.h>

static const uint8_t USER_DS = 0x1B;
static const uint8_t USER_CS = 0x23;
static const uint8_t TSS_SEL = 0x28;

extern uint64_t kernel_end;
extern uint8_t kernel_stack_top;

// everything here is for user mode stuff
extern void enter_user_mode(uint64_t entry_point, uint64_t user_stack, uint16_t user_cs, uint16_t user_ds);
extern uint8_t __user_text_start[];
extern uint8_t __user_text_end[];

// so we know where we should resume our kernel
extern void resume_kernel(void);

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

void terminal_kernel() {
    terminal_init();

    // just a msg
    if (DISPLAY_STATE == STATE_VGA) {
        printf("[terminal] cursors and color set, buffer set to VGA and screen cleared\n");
    } else if (DISPLAY_STATE == STATE_VBE) {
        printf("[terminal] cursors and color set, buffer set to VBE and screen cleared\n");
    }
}

void gdt_kernel() {
    printf("******GDT & TSS******\n");

    // here we get the kernel stack top
    uint64_t stack_top_addr = (uint64_t)&kernel_stack_top;
    // and then we init the gdt and tss stuff
    init_gdt(stack_top_addr);
    printf("[gdt & tss] GDT and TSS successfully loaded\n");
}

void interrupt_kernel() {
    printf("******INTERRUPTS******\n");

    idt_init();
    pic_remap(0x20, TSS_SEL);
    __asm__ volatile("sti");
    // also just a msg
    printf("[interrupts] IDT loaded, PIC remapped, interrupts enabled\n");

    // for testint purposes we register the handlers
    register_breakpoint_handler();

    // here we make a small test to see if the interrupts work
    printf("[test] triggering breakpoint\n");
    __asm__ volatile("int3");
    printf("[test] we are still alive (no kernel panic)\n");
}

void keyboard_kernel() {
    printf("******KEYBOARD******\n");

    keyboard_init();
    printf("[keyboard] irq1 registered\n");
    // then we also have a test where we stop everything and have the user type something and escape is for exiting this loop
    printf("[test] type something and press escape to stop\n");
    char typed;
    do {
        // we get a input key
        typed = keyboard_get_key();
        // and we directly print it to the terminal
        vbe_write(typed);
    } while (typed != 27); // 27 stands for the escape key
    printf("\n");
}

void timer_kernel() {
    printf("******TIMER******\n");

    timer_init(100);
    printf("[timer] timer pit initialized with 100hz\n");

    // then we wait for 300 ticks (3 seconds) so we know irq0 is active and firing
    uint64_t start = ticks;
    while (ticks - start < 300) {
        __asm__ volatile("hlt");
    }
    printf("[timer] 300 ticks passed (3 seconds) so the timer is alive!\n");
}

void memory_kernel(uint64_t mmap_addr, uint64_t mmap_count) {
    printf("******MEMORY******\n");

    printf("[memory] BIOS reported %d usable memory map entries\n", (int64_t)mmap_count);
    // before giving the mmap_addrs to the function we have to convert it
    mmap_entry_t *mmap = (mmap_entry_t *)mmap_addr;
    memory_init((uint64_t)&kernel_end, mmap, mmap_count);
    // also just a msg
    printf("[memory] heap beginning and end was set\n");
}

void filesystem_kernel(uint64_t initrd_addr) {
    printf("******FILESYSTEM******\n");

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
            vbe_write(data[i]);
        }

        printf("\n");
    } else {
        printf("[fs] %s file not found\n", query);
    }
}

void tasks_kernel() {
    printf("******TASKS******\n");

    printf("[tasks] created two test tasks which print A and B\n");
    /*task_create(task_a);
    task_create(task_b);
    task_create(task_reaper);*/
    scheduler_enable();
}

void user_mode_kernel() {
    printf("******USER MODE******\n");

    // but firstly we need to define some stuff
    uint16_t user_cs = USER_CS;     // this is for the gdt user code segement
    uint16_t user_ds = USER_DS;     // and this is for the gdt user data segment
    
    uint64_t size = __user_text_end - __user_text_start;    // we get the entire size of the user text
    uint64_t offset = (uint64_t)user_test_entry - (uint64_t)__user_text_start;  // then we get the offset of the distance between the start of our user text and the test function

    memcpy((void *)USER_CODE_BASE, __user_text_start, size);    // then we copy the memory from the start to end of the user text into the User code base

    syscall_init(resume_kernel);

    printf("[user mode] jumping into user space\n");
    enter_user_mode(USER_CODE_BASE + offset, USER_STACK_TOP, user_cs, user_ds); // and try to enter user mode
    printf("[user mode] back from user mode\n");

    return;
}

void enable_com1() {
    printf("******INIT COM1******\n");

    init_com1();
}

void enable_vbe(uint64_t vbe_info_addr) {
    printf("******INIT VBE******\n");

    // we load the vbe
    memcpy(&VBE, load_vbe(vbe_info_addr), sizeof(vbe_mode_info_t));

    // and init everything for the terminal for the vbe
    vbe_terminal_init(0xFFFFFF, 0x0, 2);

    printf("[VBE] size of vbe: %d\n", sizeof(vbe_mode_info_t));
    printf("[VBE] height: %d, width: %d, bits per pixel, %d\n", VBE.height, VBE.width, VBE.bpp);
    printf("[VBE] pitch: %d\n", VBE.pitch);
    printf("[VBE] framebuffer address: %d\n", VBE.framebuffer);

    const uint16_t pdpt_entrie_idx = compute_pdpt(VBE.framebuffer);
    const uint16_t pd_first_entry = compute_dir(VBE.framebuffer);
    printf("[VBE] PDPT entries index: %x\n", pdpt_entrie_idx);
    printf("[VBE] PD starting entry: %x\n", pd_first_entry);

    fill_dir_entries(pdpt_entrie_idx, pd_first_entry, &VBE);
    printf("[VBE] framebuffer mapped\n");

    // then we fill everything with the background color
    fill_rect(&VBE, 0, 0, 1024, 768, 0xFFFFFF);
}

void boot_into_kernel() {
    printf("[kernel] Press Escape to boot into the kernel\n");
    char typed;
    do {
        typed = keyboard_get_key();
    } while (typed != 27);

    fill_rect(&VBE, 0, 0, 1024, 768, 0xFFFFFF);
    write_logo("DOOMKERNEL");

    printf("\n");
}


void kernel_main(uint64_t mmap_addr, uint16_t mmap_count, uint64_t initrd_addr, uint64_t vbe_info_addr) {
    // for initing the com1
    enable_com1();

    // in this function we enable and print some vbe stuff
    if (vbe_info_addr != 0) {
        enable_vbe(vbe_info_addr);
        set_VBE();
    }

    // first thing we do is init the terminal
    terminal_kernel();

    // we init the gdt and tss here
    gdt_kernel();

    // here we init the entire interrupt stuff
    interrupt_kernel();

    // we need to init the keyboard here
    keyboard_kernel();

    // we init the timer here with 100hz
    timer_kernel();    

    // then we init the memory
    memory_kernel(mmap_addr, mmap_count);

    // we init the filesystem here
    filesystem_kernel(initrd_addr);

    // here we test our task scheduler by firstly making two tasks
    tasks_kernel();

    // here we will test our user mode by entering it then testing something and then returning to ring 0
    user_mode_kernel();

    // we wait for input so we can clear the screen, show the logo and boot into the kernel
    boot_into_kernel();

    // printing with our custom printf function :DD
    printf("[kernel] Successfully booted into the kernel!\n");

    // while loop so the cpu doesnt run off into memory junk
    while (1) {
        __asm__ volatile("hlt");
    }
}