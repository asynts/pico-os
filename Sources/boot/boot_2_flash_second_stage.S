.syntax unified
.cpu cortex-m0plus
.thumb

# The boot sequence is as follows:
#
#     RAM       boot_0_openocd_entry            Only when we are executing from debugger.
#                                               Jump to 'boot_1_reset'.
#
#     ROM       boot_1_reset                    Reset entry point.
#                                               We do not know the address of this function in advance, a pointer is stored at '0x00000004'.
#                                               This is burned into the chip.
#                                               Loads first 256 bytes from flash into RAM.
#
#     RAM       boot_2_flash_second_stage       Assembly entry point.
#                                               Configures the flash memory for XIP (execute-in-place).
#                                               Loads the kernel into memory.
#                                               Configures reset table.
#
#     RAM       boot_3_high_level               C++ entry point.

.section .boot_0_openocd_entry, "awx"

.global boot_0_openocd_entry
.thumb_func
boot_0_openocd_entry:
    # At address '0x00000004' there is a pointer to the boot reset handler.
    # Jump there.
    movs r1, #4
    ldr r1, [r1]
    blx r1

.section .boot_2_flash_second_stage, "awx"
_marker:

.global boot_2_flash_second_stage
.thumb_func
boot_2_flash_second_stage:
    movs r1, #13
    bkpt #0

    # FIXME: Actually implement the bootloader.

    wfi
    b boot_2_flash_second_stage

# Pad the size of this section to 256 bytes.
# This is mandatory such that the 'checksum.py' script can insert the checksum without much hastle.
.fill 256 - (. - _marker), 1, 0
