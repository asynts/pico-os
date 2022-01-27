commit c8c58c44fe5c60ce170ee65c6856f69b23ae52f6

I would expect, that the the `_entry_point` function initializes XIP.
However, this does not appear to happen.

### Notes

-   We don't enter at `_entry_point` but at `_stage2_boot`:

    ```c
    .global _stage2_boot
    .type _stage2_boot,%function
    .thumb_func
    _stage2_boot:
    ```
    https://github.com/raspberrypi/pico-sdk/blob/2062372d203b372849d573f252cf7c6dc2800c0a/src/rp2_common/boot_stage2/boot2_w25q080.S#L100-L103

-   It appears that the Raspberry Pi Pico uses the W25Q080 boot loader:

    ```c
    #define PICO_BOOT_STAGE2_CHOOSE_W25Q080 1
    ```
    https://github.com/raspberrypi/pico-sdk/blob/2062372d203b372849d573f252cf7c6dc2800c0a/src/boards/include/boards/pico.h#L67

### Ideas

### Theories

### Conclusions

-   I will have to import that `boot2_w25q080.S` file into my project.
    This seems to require expert knowledge.
