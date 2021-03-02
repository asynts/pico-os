MEMORY {
    rom (rx) : ORIGIN = 0x00000000, LENGTH = 64K
    ram (rw) : ORIGIN = 0x10000000, LENGTH = 64K
}

ENTRY(_start);

SECTIONS
{
    .text : {
        *(.init)
        *(.text*)
        *(.fini)
        *(.rodata*)

        /* This is needed for unwinding at runtime, we should be able to purge this somehow. */
        *(.eh_frame)

        /* These sections contain workarounds for missing processor features. */
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
    } > rom
    .data : {
        *(.data*)
        *(.noinit)
    } > ram
    .bss : {
        __bss_start__ = .;
        *(.bss*)
        __bss_end__ = .;
    } > ram
    /DISCARD/ : {
        /* Debugging information */
        *(.comment)
        *(.ARM*)
        *(.debug*)

        /* Why are these sections even emitted? They are not releavant for the Cortex-M0+. */
        *(.vfp11_veneer)
        *(.v4_bx)

        /* Why are these sections even emitted? We are linking statically?! */
        *(.iplt)
        *(.rel.iplt)
        *(.igot.plt)
    }
}
