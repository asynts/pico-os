For some reason, both the '.boot_2_flash_second_stage' and '.boot_0_openocd_entry' are placed in the same segment of the executable.
This makes no sense, because they (should) have different load addresses.

### Notes

### Ideas

-   We should load the vector table, this is what the chip does otherwise?

-   I think, it is possible to add a `SEGMENTS` section in the linker script.
    That could be used to force them into different segments.

### Theories

### Conclusions
