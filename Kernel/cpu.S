.syntax unified
.cpu cortex-m0plus
.thumb

.global scheduler_next
.global syscall_enter
.global syscall_return_trampoline

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

    push {r1} // slot for return value
    push {r1} // saved psp

    // void syscall_enter(FullRegisterContext*)
    blx syscall_enter

    // void syscall_return_trampoline(FullRegisterContext*)
    ldr r1, =syscall_return_trampoline
    str r1, [sp, #0x18]

    ldr r1, =0xfffffff9
    bx r1
