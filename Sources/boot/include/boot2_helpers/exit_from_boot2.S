/*
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _BOOT2_HELPER_EXIT_FROM_BOOT2
#define _BOOT2_HELPER_EXIT_FROM_BOOT2

#include "hardware/regs/m0plus.h"

check_return:
    // FIXME: Setup 'lr' for backtrace.

    // Load the vector table from '0x10000100' and jump to reset handler.
    ldr r0, =(XIP_BASE + 0x100)
    ldr r1, =(PPB_BASE + M0PLUS_VTOR_OFFSET)
    str r0, [r1]
    ldmia r0, {r0, r1}
    msr msp, r0
    bx r1

#endif
