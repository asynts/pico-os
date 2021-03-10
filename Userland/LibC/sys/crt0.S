.syntax unified
.cpu cortex-m0plus
.thumb

// [[noreturn]]
// void _start(u32 stack_top)
.global _start
.thumb_func
_start:
    // Initialize stack.
    mov sp, r0

    // Clear bss section.
    ldr r0, =__bss_start__
    movs r1, #0
    ldr r2, =__bss_end__
    subs r2, r2, r0
    add r0, sb
    bl memset

    // FIXME: There is a ton of stuff that has to happen here.

    movs r0, #0
    movs r1, #0
    bl main
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
