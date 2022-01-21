.syntax unified
.cpu cortex-m0plus
.thumb

.section .boot_2_flash_second_stage, "awx"
_marker:

# The boot sequence is as follows:
#
#     ROM       boot_1_reset                    Reset entry point.
#                                               This is burned into the chip.
#                                               Loads first 256 bytes from flash into RAM.
#
#     RAM       boot_2_flash_second_stage       Assembly entry point.
#                                               Configures the flash memory for XIP (execute-in-place).
#                                               Loads the kernel into memory.
#                                               Configures reset table.
#
#     RAM       boot_3_high_level               C++ entry point.

.global boot_2_flash_second_stage
.thumb_func
boot_2_flash_second_stage:
    movs r1, #13

    # FIXME: Actually implement the bootloader.

    wfi
    b boot_2_flash_second_stage

# Pad the size of this section to 256 bytes.
# This is mandatory such that the 'checksum.py' script can insert the checksum without much hastle.
.fill 256 - (. - _marker), 1, 0
