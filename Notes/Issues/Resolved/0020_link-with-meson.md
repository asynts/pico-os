commit 371cc0b79df4b52b36a308426302c56358860558

I am trying to use meson build to link the system.
However, I am having some trouble after starting to use the boot loader from `pico-sdk` directly.

### Notes

-   When meson build links the system with:

    ```none
    arm-none-eabi-g++ \
        -o System \
        -Wl,--as-needed -Wl,--no-undefined -Wl,--orphan-handling=error \
        -nostartfiles -nodefaultlibs -nolibc -nostdlib \
        -Wl,--start-group Sources/Boot/libBoot.a -Wl,--end-group \
        -static \
        -T /home/me/dev/pico-os/Sources/link.ld
    /usr/lib/gcc/arm-none-eabi/bin/ld: warning: cannot find entry symbol _entry_point; defaulting to 0000000010000000
    ```

    I am informed that the `_entry_point` symbol isn't defined and the result doesn't contain any symbols.

-   If I manually list all the files in the `libBoot.a` library:

    ```none
    $ ar t Sources/Boot/libBoot.a
    Sources/Boot/libBoot.a.p/boot_3_vectors.S.o
    Sources/Boot/libBoot.a.p/boot_4_load_kernel.cpp.o
    Sources/Boot/../../../Extern/boot.elf
    ```

    and then manually link them:

    ```none
    $ arm-none-eabi-g++ \
        -o System \
        -Wl,--as-needed -Wl,--no-undefined -Wl,--orphan-handling=error \
        -nostartfiles -nodefaultlibs -nolibc -nostdlib \
        -Wl,--start-group \
            Sources/Boot/libBoot.a.p/boot_3_vectors.S.o \
            Sources/Boot/libBoot.a.p/boot_4_load_kernel.cpp.o \
            Sources/Boot/../../../Extern/boot.elf \
        -Wl,--end-group \
        -static \
        -T /home/me/dev/pico-os/Sources/link.ld
    /usr/lib/gcc/arm-none-eabi/bin/ld: cannot use executable file 'Sources/Boot/../../../Extern/boot.elf' as input to a link
    collect2: error: ld returned 1 exit status
    ```

-   The result was still empty, nothing except `boot_1_debugger` was defined.

### Ideas

### Theories

### Conclusions

-   Instead of copying the binary file, I am copying the assembly file.
    This makes it possible for me to change a few things.

    This also solved an issue where `boot.elf` was condidered an executable.
    I don't understand why the linker didn't complain here.

-   On top of that, the linker discarded almost everything because from it's perspective only `boot_1_debugger` was the entry point and
    it didn't reference anything.

    To resolve this issue, I added manual `.global` statements that make the linker aware of these references.
