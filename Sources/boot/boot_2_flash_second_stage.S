.syntax unified
.cpu cortex-m0plus
.thumb

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
