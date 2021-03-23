ENTRY(_start);

/* FIXME: Keep debugging information! */

PHDRS {
    text PT_LOAD;
    data PT_LOAD;
}

SECTIONS
{
    . = ALIGN(4);
    .text : {
        *(.init)
        *(*.text*)
        *(.fini)
        *(.rodata*)

        __preinit_array_start = .;
        KEEP(*(.preinit_array*))
        __preinit_array_end = .;

        __init_array_start = .;
        KEEP(*(.init_array*))
        __init_array_end = .;

        __fini_array_start = .;
        KEEP(*(.fini_array*))
        __fini_array_end = .;
    } :text
    . = ALIGN(4);
    .data : {
        *(.data*)
    } :data
    . = ALIGN(4);
    .bss (NOLOAD) : {
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } :data
    . = ALIGN(4);
    .heap (NOLOAD) : {
        __heap_start__ = .;
        . += 0x10000;
        __heap_end__ = .;
    } :data
    . = ALIGN(8);
    .stack (NOLOAD) : {
        . += 0x10100;
    } :data
}
