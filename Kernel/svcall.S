.syntax unified
.cpu cortex-m0plus
.thumb

.global syscall_handler

// [[noreturn]]
// void isr_svcall(u32 syscall, u32 arg1, u32 arg2, u32 arg3)
.global isr_svcall
.thumb_func
isr_svcall:
    // Hand over execution to the system call handler
    push {lr}
    push {lr}
    push {lr}

    bl syscall_handler

    str r0, [sp, #4]
    str r1, [sp, #8]
    pop {r0}
    mov lr, r0

    // Note: r0 is saved in [sp]
    // Note: r1 is saved in [sp+4]

    // Obtain stack pointer
    mrs r0, control
    isb
    movs r1, #2
    ands r0, r0, r1
    beq 2f
1:
    mrs r0, msp
    isb
    b 3f
2:
    mrs r0, psp
    isb
3:

    // Note: r0 is saved in [sp]
    // Note: r1 is saved in [sp+4]

    // Forward return values to caller
    str r2, [r0, #8]
    str r3, [r0, #12]
    mov r2, r0
    pop {r0, r1}
    str r0, [r2, #0]
    str r1, [r2, #4]

    bx lr
