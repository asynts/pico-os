commit a0174cb1dc4e77a09b1c40c6b18d68bab7dfd384

When I use a constant with `ldr r1, =1` it is placed at `0x100` which is outside the boot sector.

### Notes

-   I thought, that the assembler would understand the padding as text and try to place it after it.
    However, marking it with `.type _padding, %object` does not help.

### Ideas

-   I could write a linker script that does one thing: `. = ALIGN(256)`.

### Theories

-   I suspect, that the `ldr r1, =1` is processed very late and the padding is computed after.
