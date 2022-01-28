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

-   Load `.data` and `.bss`.

### Bugs

-   After closing the debugger, we see the following message:

    ```none
    ./Scripts/debugger.sh: line 25: -x: command not found
    ```
