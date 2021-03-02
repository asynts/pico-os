ENTRY(_start);

SECTIONS
{
    /* These sections will remain in FLASH. They will be accessed relative to PC. */
    .text : {
        . = ALIGN(4);
        *(.init)
        . = ALIGN(4);
        *(.text)
        . = ALIGN(4);
        *(.fini)
    }
    .rodata : {
        . = ALIGN(4);
        *(.rodata)

        . = ALIGN(4);
        SORT(CONSTRUCTORS)
    }

    /* These sections will be copied into RAM. They will be accessed relative to SB. */
    .data : {
        . = ALIGN(4);
        *(.data)
    }
    .bss : {
        . = ALIGN(4);
        *(.noinit)

        __bss_start__ = .;

        . = ALIGN(4);
        *(.bss)

        __bss_end__ = .;
    }

    . = ALIGN(8);
    . += 0x1000;
    . = ALIGN(8);

    _stack_top = .;
}
