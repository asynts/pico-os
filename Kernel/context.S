.syntax unified
.cpu cortex-m0plus
.thumb

.extern current_task
.extern schedule_next_task

.text

.global isr_pendsv
.type isr_pendsv,%function
.thumb_func
isr_pendsv:
    // Obtain the stack pointer of the current task.
    mrs r0, psp
    isb

    // Save registers to stack of current task.

    // FIXME: Verify that registers are pushed low to high.
    stm r0!, {r4-r7}
    mov r1, r8
    mov r2, r9
    mov r3, r10
    mov r4, r11
    mov r5, lr
    stm r0!, {r1-r5}

    // Update the stack pointer for this process.
    ldr r1, =current_task
    ldr r2, [r1, #0]
    str r0, [r2]

    // Ask the kernel for the next task.
    stm sp!, {r1}
    bl schedule_next_task
    ldm sp!, {r1}

    // Obtain the stack pointer of the next task.
    ldr r2, [r1, #0]
    ldr r0, [r2]

    // Restore registers from stack of next task.

    // FIXME: Verify that registers are pushed low to high.
    ldm r0!, {r4-r7}
    ldm r0!, {r1-r3}
    mov r8, r1
    mov r9, r2
    mov r10, r3
    ldm r0!, {r1-r2}
    mov r11, r1
    mov lr, r2

    // Hand over execution to next task.
    msr psp, r0
    isb
    bx r14
