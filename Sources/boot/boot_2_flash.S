/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Copyright (c) 2022 Paul Scharnofske
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "include/regs/pads_qspi.h"

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
    // Setup pad configuration.
    ldr r3, =PADS_QSPI_BASE
    movs r0, #(2 << PADS_QSPI_GPIO_QSPI_SCLK_DRIVE_LSB | PADS_QSPI_GPIO_QSPI_SCLK_SLEWFAST_BITS)
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SCLK_OFFSET]
    ldr r0, [r3, #PADS_QSPI_GPIO_QSPI_SD0_OFFSET]
    movs r1, #PADS_QSPI_GPIO_QSPI_SD0_SCHMITT_BITS
    bics r0, r1
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SD0_OFFSET]
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SD1_OFFSET]
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SD2_OFFSET]
    str r0, [r3, #PADS_QSPI_GPIO_QSPI_SD3_OFFSET]

    bkpt #0

/* Pad the size of this section to 256 bytes. */
.fill 256 - (. - _marker), 1, 0
