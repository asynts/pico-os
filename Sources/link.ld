/*
There are a few unusual things about this script.
The bootloader and all writable sections are stored in flash but are copied into RAM.
Therefore, during linking they need to be in a very different location from where they are stored.
*/

MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2M
    RAM(rwx) : ORIGIN = 0x20000000, LENGTH = 256K
}

ENTRY(boot_2_flash_second_stage);

SECTIONS
{
    .boot_2_flash_second_stage : {
        __boot_2_flash_second_stage_start = .;

        KEEP (*(.boot_2_flash_second_stage))

        /* FIXME: Add the checksum. */
        . = ALIGN(256);
        __boot_2_flash_second_stage_end = .;
    } > RAM AT > FLASH

    ASSERT(__boot_2_flash_second_stage_end - __boot_2_flash_second_stage_start == 256,
        "assertion: '.boot_2_flash_second_stage' must be exactly 256 bytes.")

    .text : {
        __text_start__ = .;

        /* FIXME: There are a ton of other sections that need to stay. */
        *(.text*)

        . = ALIGN(4);
        __text_end__ = .;
    } > FLASH

    .rodata : {
        __rodata_start__ = .;

        /* FIXME: There are a ton of other sections that need to stay. */
        *(.rodata*)

        . = ALIGN(4);
        __rodata_end__ = .;
    } > FLASH

    .data : {
        __data_start__ = .;

        /* FIXME: There are a ton of other sections that need to stay. */
        *(.data*)

        . = ALIGN(4);
        __data_end__ = .;
    } > RAM AT > FLASH

    .bss : {
        __bss_start__ = .;

        /* FIXME: There are a ton of other sections that need to stay. */
        *(.bss*)

        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM

    /* FIXME: We are missing tons of ARM specific stuff. */

    __end__ = .;
}
