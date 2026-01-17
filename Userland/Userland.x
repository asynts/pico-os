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
        __libc_static_base = .;
        *(.data*)
    } :data
    . = ALIGN(4);
    .bss (NOLOAD) : {
        __libc_bss_start = .;
        *(.bss*)
        __libc_bss_end = .;
    } :data
    . = ALIGN(4);
    .heap (NOLOAD) : {
        __libc_heap_start = .;
        . += 0x1000;
        __libc_heap_end = .;
    } :data
    . = ALIGN(8);
    .stack (NOLOAD) : {
        . += 0x1100;
    } :data
}
