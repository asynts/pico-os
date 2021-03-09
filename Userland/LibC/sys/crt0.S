.syntax unified
.cpu cortex-m0plus
.thumb

// [[noreturn]]
// void _override_start(u32 stack_top)
.global _override_start
_override_start:
    // Initialize stack.
    mov sp, r0                  // FIXME: This is a HardFault?
    mov r3, r0
    bl _stack_init

    // Clear bss section.
    ldr r0, =__bss_start__
    movs r1, #0
    ldr r2, =__bss_end__
    subs r2, r2, r0
    bl memset

    // FIXME: There is a ton of stuff that has to happen here.

    bl main
    bl exit
