ENTRY(kernel_main)
OUTPUT_FORMAT("binary")

SECTIONS
{
    . = 0x8000;

    .text : {
        *(.text)
    }
    .rodata : {
        *(.rodata)
    }
    .data : {
        *(.data)
    }
    .bss : {
        bss_start = .;
        *(COMMON)
        *(.bss)
        bss_end = .;
    }

    /* Reserve 16 KB for the initial kernel stack */
    . = ALIGN(16);
    kernel_stack_bottom = .;
    . += 0x4000; /* 16 KB */
    kernel_stack_top = .;

    kernel_end = .;
}