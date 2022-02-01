commit bb2abeb5f2db95a48f0704a2e5b63e2d31bb9cbe

It appears that the linker doesn't set the thumb bit, when I call `boot_4_load_kernel` from assembly.

### Notes

-   I set a breakpoint at `isr_reset` and it appears that the thumb bit is not set, when we branch here:

    ```none
    (gdb) x/20i $pc
    => 0x1000020c <isr_reset>:	b.n	0x100002a2 <boot_4_load_kernel()>
    0x1000020e <isr_reset+2>:	wfi
    0x10000210 <isr_reset+4>:	movs	r0, r0
    ```

-   It turns out, that I didn't pass the `-mcpu` and `-mthumb` flags to the linker, but this doesn't appear to be the issue.

-   It seems that the linker spits out the correct symbol:

    ```none
    $ nm Build/System.elf | grep boot_4
    100002a3 T boot_4_load_kernel
    ```

    But this doesn't mean, that the relocation is resolved correctly.

### Ideas

-   What relocation is emitted.

-   Does the `b` instruction require the thumb bit?

### Theory

-   I suspect, that I need to declare the function in some special way?

-   Maybe, this isn't wrong and it is just the debugger that is messing with me.

### Conclusions
