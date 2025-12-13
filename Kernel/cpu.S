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

    // FullRegisterContext* syscall(FullRegisterContext*)
    bl syscall

    bl pop_callee_saved_registers

    msr psp, r0
    isb

    ldr r0, =0xfffffffd
    bx r0

// void restore_context_from_thread_mode(FullRegisterContext*)
.global restore_context_from_thread_mode
.thumb_func
restore_context_from_thread_mode:
    // r0 points to the FullRegisterContext (stack top)

    // Switch to PSP to use the thread's stack
    msr psp, r0
    movs r1, #2        // CONTROL.SPSEL = 1 (PSP)
    msr control, r1
    isb

    // Load High Registers & Status from Stack (Peek)
    ldr r1, [sp, #28] // xPSR
    msr apsr_nzcvq, r1

    ldr r1, [sp, #20] // LR
    mov lr, r1

    ldr r1, [sp, #16] // R12
    mov ip, r1

    // Restore Low Registers
    pop {r0, r1, r2, r3}

    // Skip R12 (4) and LR (4) - we already loaded them
    add sp, #8

    // Pop PC to jump to the thread entry point
    // Note: This leaves xPSR (4 bytes) on the stack, effectively leaking 4 bytes.
    // This is acceptable as this function is only used for the initial thread launch.
    pop {pc}

.global isr_hardfault
.thumb_func
isr_hardfault:
    mov r0, lr
    movs r1, #4
    tst r0, r1
    beq kernel_hard_fault

    // User mode fault (PSP)
    mrs r0, psp
    isb
    bl push_callee_saved_registers
    bl hard_fault_handler_cpp
    bl pop_callee_saved_registers
    msr psp, r0
    isb
    ldr r0, =0xfffffffd
    bx r0

kernel_hard_fault:
    // Kernel mode fault (MSP)
    // We pass the stack pointer to the handler
    mrs r0, msp
    mov r1, lr
    bl kernel_hard_fault_handler_cpp
    b .
