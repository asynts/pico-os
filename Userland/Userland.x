ENTRY(_start);

/* FIXME: Keep debugging information! */

PHDRS {
    text PT_LOAD;
    data PT_LOAD;
}

SECTIONS
{
    .text : {
        *(.init)
        *(*.text*)
        *(.fini)
        *(.rodata*)
        *(.glue*)

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
    .data : {
        *(.data*)
        *(.noinit)

        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
        end = .;
    } :data
    .stack : {
        . += 0x10100;
        __stack = .;
    } :data

    /DISCARD/ : {
        /* Why are these sections even emitted? They are not releavant for the Cortex-M0+. */
        *(.vfp11_veneer)
        *(.v4_bx)

        /* Why are these sections even emitted? We are linking statically?! */
        *(.iplt)
        *(.rel.iplt)
        *(.igot.plt)
    }
}
