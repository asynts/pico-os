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

### Ideas

-   Check if the first 256 bytes are loaded into RAM.

### Theories

-   The `objcopy` no longer works because the sections are generated in the wrong order.

-   The `objcopy` no longer works because some stuff goes into the wrong section.

-   Something (the literals!) are placed in the wrong section.

### Conclusions
