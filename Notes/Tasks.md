### Major

This is supposed to be a list of things that need to be done.
It is supposed to be ordered.

-   Write a bootloader into C++.

-   Implement some `putchar` equivalent.

-   Implement `sys::format`.

### Minor

-   Add documentation in all the bootloader code.

-   Make sure that the backtraces are helpful.

-   Validate linker scripts.
    Look for `KEEP`, verify that all sections are accounted for and that we don't link
    unnecessary libraries.

-   Simplify the boot procedure.

### Bugs

-   After closing the debugger, we see the following message:

    ```none
    ./Scripts/debugger.sh: line 25: -x: command not found
    ```

### Open Questions

-   How much of the following is correct and necessary?

    ```assembly
    .global boot_2_flash
    .type boot_2_flash, %function
    .thumb_func
    boot_2_flash:
    ```

-   I should read this:
    https://sourceware.org/binutils/docs/as/ARM_002dDependent.html

-   What is the difference between `arm-none-eabi-gcc` and `arm-none-eabi-ld` and similarly for `arm-none-eabi-as`?
