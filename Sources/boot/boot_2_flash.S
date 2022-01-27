/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Copyright (c) 2022 Paul Scharnofske
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

.syntax unified
.cpu cortex-m0plus
.thumb

// The linker script will link this at '0x20041f00' but put it at '0x10000000'.
// The bootrom will load this into memory at the former address and hand over to us.
.section .boot_2_flash, "awx"

_marker:

.global boot_2_flash
.type boot_2_flash, %function
boot_2_flash:
    bkpt #0

/* Pad the size of this section to 256 bytes. */
.fill 256 - (. - _marker), 1, 0
