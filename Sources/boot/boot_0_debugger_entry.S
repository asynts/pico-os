.syntax unified
.cpu cortex-m0plus
.thumb

.section .boot_0_debugger_entry, "awx"

.global boot_0_debugger_entry
.thumb_func
boot_0_debugger_entry:
    # At address '0x00000004' there is a pointer to the boot reset handler.
    # Jump there.
    movs r1, #4
    ldr r1, [r1]
    blx r1
