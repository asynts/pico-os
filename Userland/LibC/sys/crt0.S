.syntax unified
.cpu cortex-m0plus
.thumb

// The CortexM0+ processor does not have a memory management unit (MMU).
// Therefore, all userland executables need to be position independent.

// Use 'r9' register as static base 'sb' register (-frwpi)
.eabi_attribute Tag_ABI_PCS_R9_use, 1
// Mutable data must be accessed relative to 'sb' register (-frwpi)
.eabi_attribute Tag_ABI_PCS_RW_data, 2
// Immuatable data must be accessed relative to program counter (-fropi)
.eabi_attribute Tag_ABI_PCS_RO_data, 1

// void _init(void)
.extern _init

// void _fini(void)
.extern _fini

// [[noreturn]]
// void _start(u32 stack_top)
.global _start
.thumb_func
_start:
    push {r0, r1, r2}
    bl _init
    pop {r0, r1, r2}
    bl main

    push {r0}
    bl _fini
    pop {r0}

    bl exit

// void _syscall(u32 syscall, u32 arg1, u32 arg2, u32 arg3)
.global _syscall
.thumb_func
_syscall:
    svc #0
    bx lr

// u32 _static_base()
.global _static_base
.thumb_func
_static_base:
    mov r0, sb
    bx lr

// u32 _pc_base()
.global _pc_base
.thumb_func
_pc_base:
    mov r0, pc
    subs r0, r0, #3

    ldr r1, =_pc_base

    subs r0, r0, r1

    bx lr
