commit 266e0ef66d120d4e167f61cbdea12e30eb39cedf

For some reason I get "You may have requested too many hardware breakpoints/watchpoints." in GDB when I use watchpoints.

### Notes

-   Something is seriously wrong with GDB:

    ```none
    monitor reset init
    load
    hbreak *0x000000ef
    run
    watch *(unsigned*)0x20000000
    continue
    ```

    ```none
    Error: no breakpoint at address 0x000000ee found
    Error: Duplicate Breakpoint address: 0x000000ef (BP 211)
    ```

    The `(unsigned*)` cast was added because I suspected that the watchpoint tried to watch a too large region.

-   If I avoid the thumb bit, this seems to avoid some issues.
    However, I still have issues with the watchpoint:

    ```none
    monitor reset init
    load
    hbreak *0x000000ee
    run
    watch *(unsigned*)0x20000000
    continue
    ```

    ```none
    Error: no watchpoint at address 0x20000000 found
    ```

    I was messing with this because I suspected that either openocd or GDB had a hickup because of the thumb bit.
    The least-significant bit of each jump target needs to be set to indicate that it should jump there in thumb mode.

### Ideas

-   What happens if the boot-rom is unable to validate the checksum?
    Maybe it just resets the chip, this would explain the behaviour.

-   Maybe I can get debugging symbols for the bootrom?

### Theories

-   I suspect that the chip is reset at some point and this really messes up the debugging experience.

### Conclusions
