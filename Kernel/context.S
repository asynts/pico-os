.syntax unified
.cpu cortex-m0plus
.thumb

// u8* scheduler_next(u8*)
.extern scheduler_next

// void isr_pendsv()
.global isr_pendsv
.thumb_func
isr_pendsv:
    mrs r0, psp

.macro push_register register
    mov r1, \register
    str r1, [r0, #-4]!
.endm
.macro pop_register register
    ldr r1, [r0], #4
    mov \register, r1
.endm

    push_register r4
    push_register r5
    push_register r6
    push_register r7
    push_register r8
    push_register r9
    push_register r10
    push_register r11

    bx scheduler_next

    pop_register r11
    pop_register r10
    pop_register r9
    pop_register r8
    pop_register r7
    pop_register r6
    pop_register r5
    pop_register r4

    // FIXME: Do we need to do something about LR?

    bx lr
