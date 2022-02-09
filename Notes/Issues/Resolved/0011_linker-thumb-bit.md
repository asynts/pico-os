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

-   It appears that the `b` instruction doesn't require the thumb bit, only the `bx` instruction needs this.
    I've tested this by compiling the following assembly file:

    ```assembly
    .syntax unified
    .cpu cortex-m0plus
    .thumb

    .global foo
    .type foo, %function
    .thumb_func
    foo:
        ldr r0, =foo
        bx r0

    .global bar
    .type bar, %function
    .thumb_func
    bar:
        b bar
    ```

    ```none
    $ arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -ffreestanding -nostartfiles foo.S
    $ arm-none-eabi-objdump -d a.out
    a.out:     file format elf32-littlearm


    Disassembly of section .text:

    00008000 <foo>:
        8000:	4801      	ldr	r0, [pc, #4]	; (8008 <bar+0x4>)
        8002:	4700      	bx	r0

    00008004 <bar>:
        8004:	e7fe      	b.n	8004 <bar>
        8006:	0000      	.short	0x0000
        8008:	00008001 	.word	0x00008001
    ```

    Notice, that the `bx` instruction jumps has the thumb bit set, while the other one doesn't have it.

### Ideas

### Theory

### Conclusions

-   The `b` instruction does not require the thumb bit.
