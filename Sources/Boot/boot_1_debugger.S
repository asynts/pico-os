.syntax unified
.cpu cortex-m0plus
.thumb

.text

// When we use 'continue' in the debugger, we do not initialize the chip properly.
// Instead we jump to the entry address which is 'boot_1_debugger'.
// Here, we just trigger a breakpoint to prevent the user from doing something else.
//
// The system should be run with:
//
//     monitor reset init
//     load
//     run
//
// instead of:
//
//     monitor reset init
//     load
//     continue
.global boot_1_debugger
.type boot_1_debugger, %function
.thumb_func
boot_1_debugger:
    bkpt #0
    b boot_1_debugger

// The linker doesn't understand how the boot process works.
// It will see that 'boot_2_flash' is never called and simply discard it.
// We need to reference it from here, because 'boot_1_debugger' is the entry symbol.
.global boot_2_flash

literals:
.ltorg
.end
