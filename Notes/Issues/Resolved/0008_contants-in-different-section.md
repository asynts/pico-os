commit a0174cb1dc4e77a09b1c40c6b18d68bab7dfd384

When I use a constant with `ldr r1, =1` it is placed at `0x100` which is outside the boot sector.

### Notes

-   I thought, that the assembler would understand the padding as text and try to place it after it.
    However, marking it with `.type _padding, %object` does not help.

-   By using `objcopy --pad-to` to do the padding instead of the `.fill` in Assembly, it works.

### Ideas

### Theories

### Conclusion

-   I conclude, that the data-in-code is processed very late and after the `.fill`.
    Therefore, we need to do the padding afterwards with `objcopy` for example.
