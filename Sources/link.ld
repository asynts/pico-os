/* FIXME: Almost all the comments are outdated. */

MEMORY
{
    ROM     (r  ) : ORIGIN = 0x00000000, LENGTH = 16K
    FLASH   (rx ) : ORIGIN = 0x10000000, LENGTH = 2M
    RAM     (rwx) : ORIGIN = 0x20000000, LENGTH = 256K
    BOOTRAM (rwx) : ORIGIN = 0x20041f00, LENGTH = 256
}

ENTRY(boot_1_debugger);

SECTIONS
{
    /* FIXME: There are a ton of other sections that need to stay. */

    .boot_2_flash : {
        KEEP (*(.boot_2_flash))
    } > BOOTRAM AT > FLASH

    .text : {
        __text_start__ = .;

        *(.vectors)
        *(.text*)
        *(.rodata*)

        . = ALIGN(4);
        __text_end__ = .;
    } > FLASH AT > FLASH

    .data : {
        __data_start__ = .;

        *(.data*)

        . = ALIGN(4);
        __data_end__ = .;
    } > RAM AT > FLASH
    __data_lma__ = LOADADDR(.data);

    .bss (NOLOAD) : {
        __bss_start__ = .;

        *(.bss*)

        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM AT > RAM

    .noinit (NOLOAD) : {
        __noinit_start__ = .;
        *(.noinit*)

        . = ALIGN(4);
        __noinit_end__ = .;
    } > RAM AT > RAM

    .stack (NOLOAD) : {
        . += 0x1000;
        . = ALIGN(8);
        boot_stack_top = .;
    }

    __end__ = .;
}
