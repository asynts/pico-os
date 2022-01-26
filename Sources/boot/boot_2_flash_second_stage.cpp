export module boot:flash_second_stage;

import boot:high_level;

// This function is loaded from FLASH into RAM and is then executed at '0x20041f00'.
// The following initializatino steps must occur:
//
// -   We must setup the FLASH for XIP (execute-in-place).
//
// -   We must setup the interrupt handlers.
//
// -   We must load the kernel into memory and initialize the sections appropriately.

#define BOOTSECTOR __attribute__((section(".boot_2_flash_second_stage")))

extern "C" BOOTSECTOR
void boot_2_flash_second_stage() {
    asm volatile("bkpt #0");
}
