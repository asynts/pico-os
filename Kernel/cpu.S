.syntax unified
.cpu cortex-m0plus
.thumb

.global scheduler_next
.global syscall

// u8* push_callee_saved_registers(u8 *stack)
.thumb_func
push_callee_saved_registers:

.macro push_register register
    mov r1, \register
    subs r0, r0, #4
    str r1, [r0]
.endm

    push_register r4
    push_register r5
    push_register r6
    push_register r7
    push_register r8
    push_register r9
    push_register r10
    push_register r11

    bx lr

// u8* pop_callee_saved_registers(u8 *stack)
.thumb_func
pop_callee_saved_registers:

.macro pop_register register
    ldr r1, [r0]
    adds r0, r0, #4
    mov \register, r1
.endm

    pop_register r11
    pop_register r10
    pop_register r9
    pop_register r8
    pop_register r7
    pop_register r6
    pop_register r5
    pop_register r4

    bx lr

.global isr_pendsv
.thumb_func
isr_pendsv:
    // FIXME: We can not check the stack flag in control, because we are in the interrupt handler!

    // The first time PendSV runs, the corresponding thread will still use the main
    // stack pointer. However, 'scheduler_next' will never return the main stack pointer.
    mrs r1, control
    movs r2, #2
    ands r2, r1, r2
    beq 1f

    mrs r0, psp
    b 2f
1:
    mrs r0, msp
2:

    bl push_callee_saved_registers

    // FullRegisterContext* scheduler_next(FullRegisterContext*)
    bl scheduler_next

    bl pop_callee_saved_registers

    msr psp, r0
    isb

    ldr r0, =0xfffffffd
    bx r0

.global isr_svcall
.thumb_func
isr_svcall:
    mrs r0, psp
    isb

    bl push_callee_saved_registers

    // FullRegisterContext* syscall(FullRegisterContext*)
    bl syscall

    bl pop_callee_saved_registers

    msr psp, r0
    isb

    ldr r0, =0xfffffffd
    bx r0

// void context_switch_from_thread_mode(FullRegisterContext*)
.global context_switch_from_thread_mode
.thumb_func
context_switch_from_thread_mode:
    bl pop_callee_saved_registers

    mov sp, r0

    // FIXME: This is quite a mess, try to simplify?

    ldr r0, [sp, #0x10]
    mov ip, r0

    ldr r0, [sp, #0x14]
    mov lr, r0

    ldr r0, [sp, #0x1c]
    msr apsr_nzcvq, r0
    isb

    // FIXME: Do pop/str modify APSR?

    pop {r0, r1, r2, r3}
    str r0, [sp, #0]
    str r1, [sp, #4]
    pop {r0, r1}
    pop {pc}
