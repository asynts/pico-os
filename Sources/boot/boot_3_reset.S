/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Copyright (c) 2022 Paul Scharnofske
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#define PPB_BASE 0xe0000000
#define M0PLUS_VTOR_OFFSET 0xed08

.syntax unified
.cpu cortex-m0plus
.thumb

// The linker will put this at 0x10000100, right after the initialization logic.
// We jump here right after initializing the flash.
.section .reset, "ax"

// This must be at the start of this section.
.global boot_3_reset
.type boot_3_reset, %function
.thumb_func
boot_3_reset:
    // Tell the processor about our vector table.
    ldr r0, =boot_vector_table

    // FIXME: This should be 'ldr r1, =(PPB_BASE + M0PLUS_VTOR_OFFSET)' but that doesn't compile.
    ldr r1, =0xe000ed08

    str r0, [r1]

    // Obtain the stack pointer and address of 'isr_reset' from vector table.
    ldmia r0!, {r1, r2}

    // Setup stack.
    msr msp, r1

    // Jump to 'isr_reset'.
    blx r2
