commit 26eea8f62d5c643dce3f7c2bd2792b5ac2f5941b

After importing the code from the boot loader, I am no longer able to reach the flash boot.

### Notes

-   I get stuck at address `0x00001bd0`.
    If I recall correctly, this means that the checksum is incorrect.

-   I suspected, that we are missing some things that are defined in the command line.
    After inspecting an example build with the SDK, the following flags are used:

    ```none
    -DPICO_BOARD=pico -DPICO_BUILD=1 -DPICO_NO_HARDWARE=0 -DPICO_ON_DEVICE=1
    ```

    However, none of them appear to be referenced in the code that I imported.

-   I though that the literals were placed in a different section.
    But this is not the case, they are mearly marked with a global symbol.

-   There is an additional `.rel.boot_2_flash` section.
    This doesn't look good.

-   I tried linking it, but I can't do a complete link, because the linker doesn't know how to deal with the result.
    And if I use `--relocatable`, then these sectons are not resolved.

    If we don't have any global symbols in there, we have no problem.
    I wasn't able to find any solution that explains how to resolve some relocations.

    https://stackoverflow.com/questions/35324636/resolve-relative-relocations-in-partial-link
    https://stackoverflow.com/questions/29391965/what-is-partial-linking-in-gnu-linker/53959624#53959624

### Ideas

### Theories

### Conclusions

-   I removed the `.global` in the Assembly, this prevents the linker from emitting the relocations.
    This is a workaround, I was not able to convince the linker to resolve the symbols without discarding them.
