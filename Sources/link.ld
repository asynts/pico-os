/*
There are a few unusual things about this script:

-   The bootloader and all writable sections are stored in flash but are loaded into RAM.
    Therefore, we need to tell the linker where to find them at runtime and communicate to the loader, where to load it.

-   There are two different ways, we can boot the system:

     1. If we just reset the chip, it grabs the vector from '0x00000000' and then triggers the reset vector.
        We call this function 'boot_1_reset'.

     2. If we run in GDB, we load the executable ourselves with the 'load' command.
        Since we want to be able to reproduce the normal behaviour, we enter at 'boot_0_openocd_entry' which
        manually jumps to 'boot_1_reset'.

        FIXME: We should load the vector table too?!
*/

MEMORY
{
    ROM   (r  ) : ORIGIN = 0x00000000, LENGTH = 16K
    FLASH (rx ) : ORIGIN = 0x10000000, LENGTH = 2M
    RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 256K
}

/*
This is only inspected by 'openocd' or rather 'gdb'.
When we reset the chip normally, it doesn't even know this value.
In both cases, we execute 'boot_1_reset' directly or indirectly.
*/
ENTRY(boot_0_openocd_entry);

SECTIONS
{
    .boot_2_flash_second_stage : {
        . = ORIGIN(RAM);

        __boot_2_flash_second_stage_start = .;

        KEEP (*(.boot_2_flash_second_stage))
        . = ALIGN(256);

        __boot_2_flash_second_stage_end = .;
    } > RAM AT > FLASH

    .boot_0_openocd_entry : {
        KEEP (*(.boot_0_openocd_entry))
    } > RAM AT > RAM

    ASSERT(__boot_2_flash_second_stage_start == ORIGIN(RAM),
        "assertion: '.boot_2_flash_second_stage' must be placed at address '0x20000000'.")

    ASSERT(__boot_2_flash_second_stage_end - __boot_2_flash_second_stage_start == 256,
        "assertion: '.boot_2_flash_second_stage' must be exactly 256 bytes.")

    .text : {
        __text_start__ = .;

        /* FIXME: There are a ton of other sections that need to stay. */
        *(.text*)

        . = ALIGN(4);
        __text_end__ = .;
    } > FLASH AT > FLASH

    .rodata : {
        __rodata_start__ = .;

        /* FIXME: There are a ton of other sections that need to stay. */
        *(.rodata*)

        . = ALIGN(4);
        __rodata_end__ = .;
    } > FLASH AT > FLASH

    .data : {
        __data_start__ = .;

        /* FIXME: There are a ton of other sections that need to stay. */
        *(.data*)

        . = ALIGN(4);
        __data_end__ = .;
    } > RAM AT > FLASH

    .bss (NOLOAD) : {
        __bss_start__ = .;

        /* FIXME: There are a ton of other sections that need to stay. */
        *(.bss*)

        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM AT > RAM

    /* FIXME: We are missing tons of ARM specific stuff. */

    __end__ = .;
}
