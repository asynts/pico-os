.syntax unified
.cpu cortex-m0plus
.thumb

// void _init(void)
.extern _init

// void _fini(void)
.extern _fini

// [[noreturn]]
// void _start(u32 stack_top)
.global _start
.thumb_func
_start:
    // Initialize stack.
    mov sp, r0

    bl _init

    movs r0, #0
    movs r1, #0
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
