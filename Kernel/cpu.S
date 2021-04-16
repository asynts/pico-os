.syntax unified
.cpu cortex-m0plus
.thumb

.global scheduler_next
.global syscall_handler

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
    mrs r0, psp
    isb

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

    push {r0} // slot for return value
    push {r0} // saved psp

    // i32 syscall_handler(FullRegisterContext*)
    blx syscall_handler

    str r0, [sp, #4]
    pop {r0}

    bl pop_callee_saved_registers

    msr psp, r0
    isb

    pop {r1}
    str r1, [r0, #0]

    ldr r1, =0xfffffffd
    bx r1
