.syntax unified
.cpu cortex-m0plus
.thumb

.extern current_task
.extern prepare_next_task

.text

// void isr_pendsv(void)
.global isr_pendsv
.type isr_pendsv,%function
.thumb_func
isr_pendsv:
    // If there is no task running, skip saving.
    ldr r1, =current_task
    cmp r1, #0
    beq 1f

    // FIXME: Maybe I have to dereference current_task twice? But why?

    // Obtain the stack pointer of the current task.
    mrs r0, psp
    isb

    // FIXME: Somehow there is a scenario where (current_task != nullptr)
    //        and (psp != nullptr) but still (*psp) is a hardfault.

    // Save registers to stack of current task.
    subs r0, r0, #36
    stmia r0!, {r4-r7}
    mov r1, r8
    mov r2, r9
    mov r3, r10
    mov r4, r11
    mov r5, lr
    stmia r0!, {r1-r5}

    // Update the stack pointer for this process.
    ldr r1, =current_task
    str r0, [r1]

1:
    // Ask the kernel for the next task.
    mov r1, lr
    push {r1}
    bl prepare_next_task
    pop {r1}
    mov lr, r1

    // If there is no task being scheduled, return.
    ldr r1, =current_task
    cmp r1, #0
    bne 2f
    bx lr

2:
    // Obtain the stack pointer of the next task.
    ldr r0, [r1]

    // Restore registers from stack of next task.
    subs r0, r0, #36
    ldmia r0!, {r1-r5}
    mov r8, r1
    mov r9, r2
    mov r10, r3
    mov r11, r4
    mov lr, r5
    ldmia r0!, {r4-r7}

    // Hand over execution to next task.
    msr psp, r0
    isb
    bx lr
