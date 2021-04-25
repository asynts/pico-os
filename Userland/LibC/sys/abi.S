.syntax unified
.cpu cortex-m0plus
.thumb

// void* memmove(void *dest, const void *src, size_t count)
.extern memmove

// u32 _pc_base()
.extern _pc_base

// void __aeabi_memmove(void *dest, const void *src, size_t n)
.global __aeabi_memmove
.thumb_func
__aeabi_memmove:
    push {r0} // slot

    mov r3, lr
    push {r0, r1, r2, r3}

    bl _pc_base

    ldr r1, =memmove
    add r1, r1, r0

    str r1, [sp, #16]

    pop {r0, r1, r2, r3}
    mov lr, r3

    pop {r3}
    bx r3
