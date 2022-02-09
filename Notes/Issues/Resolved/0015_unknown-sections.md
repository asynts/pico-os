commit 0ae8619c8fd505456abeafb52bf3bf70a4651eea

After adding `-Wl,--orphan-handling=error`, the linker is now complaining about some sections that are not mentioned in my linker script.

### Notes

-   After adding `-Wl,--orphan-handling=error` to the link step, the linker is now complaining because I am not handling some special sections:

    ```none
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.glue_7' from `linker stubs'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.glue_7t' from `linker stubs'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.vfp11_veneer' from `linker stubs'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.v4_bx' from `linker stubs'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.iplt' from `Build/boot/boot_3_vectors.S.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.rel.iplt' from `Build/boot/boot_3_vectors.S.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.igot.plt' from `Build/boot/boot_3_vectors.S.o'
    collect2: error: ld returned 1 exit status
    ```

-   It appears that the linker is adding some "stubs" that help with some operations that are not natively supported by the processor.

-   I created a minimal example:

    ```assembly
    # foo.S

    .syntax unified
    .cpu cortex-m0plus
    .thumb

    .global foo
    .type foo, %function
    .thumb_func
    foo:
        # tailcall
        b bar
    ```

    ```assembly
    # bar.S

    .syntax unified
    .cpu cortex-m0plus
    .thumb

    .global bar
    .type bar, %function
    .thumb_func
    bar:
        bx lr
    ```

    ```none
    # link.ld

    SECTIONS {
    }
    ```

    ```none
    $ arm-none-eabi-gcc -c foo.S
    $ arm-none-eabi-gcc -c bar.S
    $ arm-none-eabi-gcc -Wl,--orphan-handling=error -T link.ld foo.o bar.o -ffreestanding -nostdlib -nostartfiles
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.glue_7' from `linker stubs'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.glue_7t' from `linker stubs'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.vfp11_veneer' from `linker stubs'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.v4_bx' from `linker stubs'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.text' from `foo.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.data' from `foo.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.bss' from `foo.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.ARM.attributes' from `foo.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.iplt' from `foo.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.rel.iplt' from `foo.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.igot.plt' from `foo.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.text' from `bar.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.data' from `bar.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.bss' from `bar.o'
    /usr/lib/gcc/arm-none-eabi/11.2.0/../../../../arm-none-eabi/bin/ld: error: unplaced orphan section `.ARM.attributes' from `bar.o'
    collect2: error: ld returned 1 exit status
    ```

    Here, we can clearly see that `foo.o` uses the procedure lookup table while `bar.o` does not.
    This confirms, that the `foo` calls `bar` dynamically.
    However, it should be possible to resolve this at runtime, therefore, the linker is configured incorrectly.

-   I've tried manually adding the sections that it's complaining about.
    It seems, that they are not used but the linker still expects them to appear.

-   I've verified that the result is statically linked, even if it complains about the missing sections.
    Linkers are weird.

### Ideas

### Theories

### Conclusions

-   The linker links some additional functions that it uses in some cases.
    For example, "veneers" are used if a jump needs to be performed that exceeds the relative jump offset.

-   `boot_3_vectors` calls `boot_4_load_kernel` dynamically and tries to use the procedure lookup table in the process.
    The solution is to simply define these sections in the linker script, the linker hopefully won't use them.
