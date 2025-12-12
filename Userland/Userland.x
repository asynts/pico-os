ENTRY(_start);

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
        . += 0x1000;
        __heap_end__ = .;
    } :data
    . = ALIGN(8);
    .stack (NOLOAD) : {
        . += 0x1100;
    } :data

    /* DWARF debug sections for GDB */
    .debug_info     0 : { *(.debug_info) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_ranges   0 : { *(.debug_ranges) }
}
