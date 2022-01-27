.syntax unified
.cpu cortex-m0plus
.thumb

// When the chip is reset, it executes from bootrom.
// That logic will load the first 256 bytes from memory into RAM and jump there.
// We link this section at the beginning of flash such that it is loaded.
.section .boot_2_flash_second_stage, "awx"
_marker:

// Notice, that this is placed at the beginning of this section.
// This is where the bootrom will jump.
.global boot_2_flash_second_stage
.type boot_2_flash_second_stage, %function
.thumb_func
boot_2_flash_second_stage:
    bkpt #0

# Pad the size of this section to 256 bytes.
# This is mandatory such that the 'checksum.py' script can insert the checksum without much hastle.
.fill 256 - (. - _marker), 1, 0
