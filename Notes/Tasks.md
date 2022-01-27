### Current

### Priority List

This is supposed to be a list of things that need to be done.
It is supposed to be ordered.

-   Write a bootloader into C++.

    -   We need to setup the interrupt handlers in `boot_2_flash_second_stage`.

    -   We need to initialize the flash in `boot_2_flash_second_stage`.

### Tweaks

-   Instead of losing the symbols in `checksum.py` I could write a linker script that keeps them.

-   Get rid of `noexcept` everywhere.

### Bugs

-   After closing the debugger, we see the following message:

    ```none
    ./Scripts/debugger.sh: line 25: -x: command not found
    ```
