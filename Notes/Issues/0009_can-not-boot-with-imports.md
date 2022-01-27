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

### Ideas

-   Check if the first 256 bytes are loaded into RAM.

### Theories

-   I suspect, that we are generating some relocations somewhere, because some macro isn't defined.
    Therefore, we change the section after padding and after computing the checksum.

### Conclusions
