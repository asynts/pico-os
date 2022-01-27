/*
We need to link early, because any relocations needs to be resolved before we compute the checksum.
Otherwise, the linker will apply them without updating the checksum.
*/

MEMORY
{
    BOOTRAM (rwx) : ORIGIN = 0x20041f00, LENGTH = 256
}

SECTIONS
{
    .boot_2_flash : {
        *(.boot_2_flash)
        *(.text)
        *(.bss)
        *(.data)

        ASSERT(. <= ORIGIN(BOOTRAM) + 252, "assertion failed: flash stage is larger than 252 bytes");

        . = ALIGN(256);
    } >BOOTRAM
}
