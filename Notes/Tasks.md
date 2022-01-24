### Current

### Priority List

This is supposed to be a list of things that need to be done.
It is supposed to be ordered.

-   Write a bootloader into C++.

### Tweaks

-   Add symbol for `boot_1_reset`.
    Maybe just `bootrom`? That would be more portable?
    I should also get rid of symbols that are defined by the linker script.

-   Run the `boot` folder into a C++20 module.

-   Move the resolved issues into another folder.

### Bugs

-   After closing the debugger, we see the following message:

    ```none
    ./Scripts/debugger.sh: line 25: -x: command not found
    ```

-   For some reason `run` doesn't work in GDB.
    I would expect that this goes through `boot_1_reset`.
