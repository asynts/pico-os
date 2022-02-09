commit 5cd44de0d58fbbb4d1231f694c970f2a3e14ad0a

When we try to hand over to C++ by jumping to `boot_4_load_kernel` in `isr_reset`, we fault.

### Notes

-   Originally, I thought, that the `b` instruction was responsible, however, it appears that any instruction causes a hard fault, even `nop`.

-   When I implemented `isr_reset` without the `decl_isr_bkpt` macro, I forgot to add the `.thumb_func` stuff.

### Ideas

### Theories

### Conclusions

-   This was caused, because I forgot to add `.thumb_func` to `isr_reset`.
    It appears that the processor expects the thumb bit to be set when it uses the vector table.
