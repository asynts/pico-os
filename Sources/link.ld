/*
There are a few unusual things about this script:

-   The bootloader and all writable sections are stored in flash but are loaded into RAM.
    Therefore, we need to tell the linker where to find them at runtime and communicate to the loader, where to load it.

-   There are two different ways, we can boot the system:

     1. If we just reset the chip, it grabs the vector from '0x00000000' and then triggers the reset vector.
        We call this function 'boot_1_reset'.

     2. If we run in GDB, we load the executable ourselves with the 'load' command.
        Since we want to be able to reproduce the normal behaviour, we enter at 'boot_0_debugger_entry' which
        manually jumps to 'boot_1_reset'.

        FIXME: We should load the vector table too?!

If the chip is reset, the boot sequence is as follows:

    ROM       boot_1_reset                  This is burned into the chip.
                                            Loads first 256 bytes from flash into RAM.

    RAM       boot_2_flash_second_stage     Configures the flash memory for XIP (execute-in-place).
                                            Loads the kernel into memory.
                                            Configures vector table.

    FLASH     boot_3_high_level             C++ entry point.

If we run in a debugger and use 'continue' to start up, the boot sequence is as follows:

    FLASH   boot_1_debugger                 Breakpoint.

If we run in a debugger and use 'run' to start up, the boot sequence is identical to the normal reset.
*/

MEMORY
{
    ROM (r  ) : ORIGIN = 0x00000000, LENGTH = 16K
    FLASH   (rx ) : ORIGIN = 0x10000000, LENGTH = 2M
    RAM     (rwx) : ORIGIN = 0x20000000, LENGTH = 256K

    /* The flash stage is loaded at this address, therefore, we must put it there. */
    BOOTRAM (rwx) : ORIGIN = 0x20041f00, LENGTH = 256
}

/*
This is only inspected by 'openocd' or rather 'gdb'.
When we reset the chip normally, it doesn't even know this value.
In both cases, we execute 'boot_1_reset' directly or indirectly.
*/
ENTRY(boot_1_debugger);

SECTIONS
{
    .boot_2_flash_second_stage : {
        __boot_2_flash_second_stage_start = .;

        KEEP (*(.boot_2_flash_second_stage))

        __boot_2_flash_second_stage_end = .;
    } > BOOTRAM AT > FLASH

    ASSERT(__boot_2_flash_second_stage_start == ORIGIN(BOOTRAM),
        "assertion: '.boot_2_flash_second_stage' must be placed at address '0x20041f00'.")

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
