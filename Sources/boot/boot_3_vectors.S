/*
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 * Copyright (c) 2022 Paul Scharnofske
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

.syntax unified
.cpu cortex-m0plus
.thumb

// The flash boot will load the vector table from here.
// It will setup the stack and jump to the 'isr_reset' routine.
.section .vectors, "ax"
.align 4

.global boot_vector_table
.type boot_vector_table, %object
boot_vector_table:
.word boot_stack_top
.word isr_reset
.word isr_nmi
.word isr_hardfault
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_svcall
.word isr_invalid // Reserved, should never fire
.word isr_invalid // Reserved, should never fire
.word isr_pendsv
.word isr_systick
.word isr_irq0
.word isr_irq1
.word isr_irq2
.word isr_irq3
.word isr_irq4
.word isr_irq5
.word isr_irq6
.word isr_irq7
.word isr_irq8
.word isr_irq9
.word isr_irq10
.word isr_irq11
.word isr_irq12
.word isr_irq13
.word isr_irq14
.word isr_irq15
.word isr_irq16
.word isr_irq17
.word isr_irq18
.word isr_irq19
.word isr_irq20
.word isr_irq21
.word isr_irq22
.word isr_irq23
.word isr_irq24
.word isr_irq25
.word isr_irq26
.word isr_irq27
.word isr_irq28
.word isr_irq29
.word isr_irq30
.word isr_irq31

.macro decl_isr_bkpt name
.weak \name
.type \name, %function
.thumb_func
\name:
    bkpt #0
.endm

// decl_isr_bkpt isr_reset
decl_isr_bkpt isr_invalid
decl_isr_bkpt isr_nmi
decl_isr_bkpt isr_hardfault
decl_isr_bkpt isr_svcall
decl_isr_bkpt isr_pendsv
decl_isr_bkpt isr_systick

decl_isr_bkpt isr_irq0
decl_isr_bkpt isr_irq1
decl_isr_bkpt isr_irq2
decl_isr_bkpt isr_irq3
decl_isr_bkpt isr_irq4
decl_isr_bkpt isr_irq5
decl_isr_bkpt isr_irq6
decl_isr_bkpt isr_irq7
decl_isr_bkpt isr_irq8
decl_isr_bkpt isr_irq9
decl_isr_bkpt isr_irq10
decl_isr_bkpt isr_irq11
decl_isr_bkpt isr_irq12
decl_isr_bkpt isr_irq13
decl_isr_bkpt isr_irq14
decl_isr_bkpt isr_irq15
decl_isr_bkpt isr_irq16
decl_isr_bkpt isr_irq17
decl_isr_bkpt isr_irq18
decl_isr_bkpt isr_irq19
decl_isr_bkpt isr_irq20
decl_isr_bkpt isr_irq21
decl_isr_bkpt isr_irq22
decl_isr_bkpt isr_irq23
decl_isr_bkpt isr_irq24
decl_isr_bkpt isr_irq25
decl_isr_bkpt isr_irq26
decl_isr_bkpt isr_irq27
decl_isr_bkpt isr_irq28
decl_isr_bkpt isr_irq29
decl_isr_bkpt isr_irq30
decl_isr_bkpt isr_irq31

.global isr_reset
.type isr_reset, %function
.thumb_func
isr_reset:
    // We do not want 'isr_reset' to show up in a backtrace, just hand over to 'boot_3_vectors'.
    b boot_3_vectors

.global boot_3_vectors
.type boot_3_vectors, %function
.thumb_func
boot_3_vectors:
    push {lr}

    // void boot_4_load_kernel(void);
    bl boot_4_load_kernel

1:
    wfi
    b 1b

    // Not reached.
    bkpt #0

literals:
.ltorg
.end
