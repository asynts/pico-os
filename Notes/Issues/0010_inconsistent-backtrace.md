commit 539779a273f4af2f1b13b1866f7f3f783403afff

When I do a backtrace in some locations, it sometimes includes '0x00000000' or is complete non-sense

### Notes

-   Backtrace in `boot_1_debugger`:

    ```none
    #0  boot_1_debugger () at Sources/boot/boot_1_debugger.S:26
    ```

-   Backtrace in `boot_2_flash`:

    ```none
    #0  boot_2_flash () at Sources/boot/boot_2_flash.S:105
    #1  0x00000000 in ?? ()
    ```

-   I thought, this may be because we pushed the `lr` register before entering the function, but this does not appear to be the case.

-   It appears that we have to use some pseudo instruction to generate unwind information for the debugger.
    https://sourceware.org/binutils/docs/as/ARM-Unwinding-Tutorial.html

-   Using `.fnstart` and `.fnend` appears to generate the information needed for exception handling, but I get issues with linking.

### Ideas

-   Try building with `-fexceptions` and use `.fnstart` with `.fnend`.

-   I could experiment with adding jumps.

-   Go though the build script and simplify.

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

### Theories

-   I suspect, that this is because there are actually two symbols there, `_marker` and `boot_2_flash`.

### Conclusions
