ENTRY(kernel_main)
OUTPUT_FORMAT("binary")

SECTIONS
{
    . = 0x8000;
    kernel_end = .;

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
        *(.bss)
    }
}