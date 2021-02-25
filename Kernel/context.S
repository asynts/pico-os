.syntax unified
.cpu cortex-m0plus
.thumb

.extern scheduler_prepare_next_task

.text

// void isr_pendsv(void)
.global isr_pendsv
.type isr_pendsv,%function
.thumb_func
isr_pendsv:
    // Are we using MSP or PSP?
    mrs r1, control
    isb
    movs r2, #2
    ands r1, r2

    // Move correct stack pointer into R0.
    cmp r1, #0
    beq 1f
    mrs r0, psp
    isb
    b 2f
1:
    mrs r0, msp
    isb
2:

    // Save registers to stack of current task.
    subs r0, r0, #40
    stmia r0!, {r4-r7}
    mov r1, r8
    mov r2, r9
    mov r3, r10
    mov r4, r11
    mov r5, lr
    mrs r6, control
    isb
    stmia r0!, {r1-r6}
    subs r0, r0, #40

    // Ask the kernel for the next task.
    bl scheduler_prepare_next_task

    // Restore registers from stack of next task.
    ldmia r0!, {r1-r6}
    mov r8, r1
    mov r9, r2
    mov r10, r3
    mov r11, r4
    mov lr, r5
    msr control, r6
    isb
    ldmia r0!, {r4-r7}

    // Are we using MSP or PSP?
    mrs r1, control
    isb
    movs r2, #2
    ands r1, r2

    // Restore the correct stack pointer.
    cmp r1, #0
    beq 1f
    msr psp, r0
    isb
    b 2f
1:
    msr psp, r0
    isb
2:

    // Hand over execution to next task.
    bx lr
