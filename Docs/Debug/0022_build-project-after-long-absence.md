commitid 7bf4e3c38c1f3a72d0639c7ab15920f850325a5f

### Notes

-   After building the project for the first time in a long time, I get the following linking error:
    ```none
    /usr/bin/arm-none-eabi-ld: error: /tmp/crt0-a6b71d.o: conflicting use of R9
    /usr/bin/arm-none-eabi-ld: failed to merge target specific data of file /tmp/crt0-a6b71d.o
    ```
    There are several other warnings, but this seems to be the actual problem.

    -   However, it seems that this register isn't actually used in this file?

    -   I did find a mention of this error message here, even with the register R9:
        https://cdn.kernel.org/pub/tools/crosstool/files/patches/binutils-2.30-nds32.diff
        `elf32_arm_merge_eabi_attributes (bfd *ibfd, struct bfd_link_info *info)`

    -   It seems that BFD stands for binary file descriptor and is used for mainpulation of object files:
        https://en.wikipedia.org/wiki/Binary_File_Descriptor_library

    -   I did find the code location here:
        https://github.com/bminor/binutils-gdb/blob/d320649e1805e5330a09b103d4ca890a12815f56/bfd/elf32-arm.c#L13933-L13940

    -   According to this document, there was a breaking change in the assembler command line options:
        https://documentation-service.arm.com/static/5e7b64627158f500bd5bcb1e?token=

        -   It seems that for "read-write position independent" code, you need to set some attributes in the assembly file
            ```none
            .eabi_attribute Tag_ABI_PCS_R9_use, 1
            .eabi_attribute Tag_ABI_PCS_RW_data, 2
            ```
            However, I have no clue what this actually means.

        -   I didn't use the `--apcs` option before, but maybe it was active implicitly?

        -   According to the dates, this migration should have happend a long time ago before I did the project.
            I don't think that I was working with an outdated version of the compiler.

    -   I found a reference to it in the addenda of the ABI specification:
        https://github.com/ARM-software/abi-aa/blob/a78fa26d95a094264233114e49f5c4e1636df7b2/addenda32/addenda32.rst#336procedure-call-related-attributes

-   This is the command that is causing issues:
    ```shell
    clang \
        --target=arm-none-eabi \
        -mcpu=cortex-m0plus \
        -std=gnu11 \
        -Og -g \
        -static \
        -nostdlib \
        -fcolor-diagnostics \
        -fropi \
        -frwpi \
        -DUSERLAND \
        -I /home/me/dev/pico-os/Userland/LibC \
        -I /home/me/dev/pico-os \
        -T /home/me/dev/pico-os/Userland/Userland.x \
        -Xlinker --nmagic \
        --sysroot=/usr/local/arm-none-eabi \
        /home/me/dev/pico-os/Userland/LibC/assert.c \
        /home/me/dev/pico-os/Userland/LibC/ctype.c \
        /home/me/dev/pico-os/Userland/LibC/dirent.c \
        /home/me/dev/pico-os/Userland/LibC/errno.c \
        /home/me/dev/pico-os/Userland/LibC/fcntl.c \
        /home/me/dev/pico-os/Userland/LibC/malloc.c \
        /home/me/dev/pico-os/Userland/LibC/readline/readline.c \
        /home/me/dev/pico-os/Userland/LibC/spawn.c \
        /home/me/dev/pico-os/Userland/LibC/stdio.c \
        /home/me/dev/pico-os/Userland/LibC/stdlib.c \
        /home/me/dev/pico-os/Userland/LibC/string.c \
        /home/me/dev/pico-os/Userland/LibC/sys/abi.c \
        /home/me/dev/pico-os/Userland/LibC/sys/crt0.S \
        /home/me/dev/pico-os/Userland/LibC/sys/crt0.c \
        /home/me/dev/pico-os/Userland/LibC/sys/stat.c \
        /home/me/dev/pico-os/Userland/LibC/sys/system.c \
        /home/me/dev/pico-os/Userland/LibC/sys/wait.c \
        /home/me/dev/pico-os/Userland/LibC/unistd.c \
        /home/me/dev/pico-os/Userland/Example.c \
        -o /home/me/dev/pico-os/Build/Userland/Example.1.elf
    ```
    I am trying to reduce it:
    ```shell
    clang \
        --target=arm-none-eabi \
        -mcpu=cortex-m0plus \
        -std=gnu11 \
        -Og -g \
        -static \
        -nostdlib \
        -fcolor-diagnostics \
        -fropi \
        -frwpi \
        -DUSERLAND \
        -I /home/me/dev/pico-os/Userland/LibC \
        -I /home/me/dev/pico-os \
        -T /home/me/dev/pico-os/Userland/Userland.x \
        -Xlinker --nmagic \
        --sysroot=/usr/local/arm-none-eabi \
        /home/me/dev/pico-os/Userland/LibC/sys/crt0.S \
        /home/me/dev/pico-os/Userland/Example.c \
        -o /home/me/dev/pico-os/Build/Userland/Example.1.elf
    ```

-   Unfortunately, the register names are not documented for the GNU assembler:
    https://sourceware.org/binutils/docs/as/ARM_002dRegs.html

-   However, the registers are documented in the ABI:
    https://github.com/ARM-software/abi-aa/blob/main/aapcs32/aapcs32.rst#611core-registers

-   I am reading the metadata from the `.eabi_attributes` back and comparing them:
    ```none
    $ arm-none-eabi-readelf --arch-specific ./crt0.o
    Attribute Section: aeabi
    File Attributes
    Tag_CPU_name: "cortex-m0plus"
    Tag_CPU_arch: v6S-M
    Tag_CPU_arch_profile: Microcontroller
    Tag_ARM_ISA_use: No
    Tag_THUMB_ISA_use: Thumb-1
    Tag_CPU_unaligned_access: None
    ~/dev/pico-os/Build (me)
    $ arm-none-eabi-readelf --arch-specific ./Example.o
    Attribute Section: aeabi
    File Attributes
    Tag_conformance: "2.09"
    Tag_CPU_name: "cortex-m0plus"
    Tag_CPU_arch: v6S-M
    Tag_CPU_arch_profile: Microcontroller
    Tag_ARM_ISA_use: No
    Tag_THUMB_ISA_use: Thumb-1
    Tag_ABI_PCS_R9_use: SB
    Tag_ABI_PCS_RW_data: SB-relative
    Tag_ABI_PCS_RO_data: PC-relative
    Tag_ABI_PCS_GOT_use: direct
    Tag_ABI_PCS_wchar_t: 4
    Tag_ABI_FP_denormal: Needed
    Tag_ABI_FP_exceptions: Unused
    Tag_ABI_FP_number_model: IEEE 754
    Tag_ABI_align_needed: 8-byte
    Tag_ABI_align_preserved: 8-byte, except leaf SP
    Tag_ABI_enum_size: int
    Tag_ABI_optimization_goals: Prefer Speed
    Tag_CPU_unaligned_access: None
    Tag_ABI_FP_16bit_format: IEEE 754
    ```

-   I need to add the following tags:
    Tag_conformance: "2.09"
    Tag_ABI_PCS_R9_use: SB
    Tag_ABI_PCS_RW_data: SB-relative
    Tag_ABI_PCS_RO_data: PC-relative
    Tag_ABI_PCS_GOT_use: direct
    Tag_ABI_PCS_wchar_t: 4
    Tag_ABI_FP_denormal: Needed
    Tag_ABI_FP_exceptions: Unused
    Tag_ABI_FP_number_model: IEEE 754
    Tag_ABI_align_needed: 8-byte
    Tag_ABI_align_preserved: 8-byte, except leaf SP
    Tag_ABI_enum_size: int
    Tag_ABI_optimization_goals: Prefer Speed
    Tag_ABI_FP_16bit_format: IEEE 754

-   Documentation about the ARM assember:
    https://documentation-service.arm.com/static/5f3e8d48b13d4764d4613a13

-   The first issue seems to be resolved with the additional `.abi_attribute` directives.
    However, I still get the other error: `dangerous relocation: unsupported relocation`

    -   This is caused by the `-frwpi` command line option

    -   This script reproduces the issue:
        ```sh
        cd /home/me/dev/pico-os/Build/Userland

        clang \
            --target=arm-none-eabi \
            -mcpu=cortex-m0plus \
            -std=gnu11 \
            -Og -g \
            -static \
            -nostdlib \
            -fcolor-diagnostics \
            -DUSERLAND \
            -I /home/me/dev/pico-os/Userland/LibC \
            -I /home/me/dev/pico-os \
            -frwpi \
            --sysroot=/usr/local/arm-none-eabi \
            /home/me/dev/pico-os/Userland/Example.c \
            -c

        arm-none-eabi-ld \
            --sysroot=/usr/local/arm-none-eabi \
            -Bstatic \
            -m armelf \
            -EL \
            -T /home/me/dev/pico-os/Userland/Userland.x \
            -L/usr/local/arm-none-eabi/lib \
            -L/usr/lib/clang/21/lib/arm-unknown-none-eabi \
            -L/usr/local/arm-none-eabi/lib \
            --nmagic \
            -z noexecstack \
            ./Example.o \
            --target2=rel \
            -o ./Example.1.elf
        ```

    -   This is a minimal example:
        ```c
        int bar;

        void baz()
        {
            bar = 0;
        }

        void _start() { }
        ```
        ```sh
        clang \
            --target=arm-none-eabi \
            -mcpu=cortex-m0plus \
            -frwpi \
            /home/me/dev/pico-os/Userland/LibC/sys/stat.c \
            -c

        arm-none-eabi-ld \
            ./stat.o \
            -o ./Example.1.elf
        ```
        ```none
        ./stat.o: in function `_start':
        stat.c:(.text+0xc): dangerous relocation: unsupported relocation
        ```

-   It seems that `-frwpi` emits a relocation that is not supported?
    ```none
    $ arm-none-eabi-readelf --relocs ./stat.o

    Relocation section '.rel.text' at offset 0x11c contains 1 entry:
    Offset     Info    Type            Sym.Value  Sym. Name
    0000000c  00000609 R_ARM_SBREL32     00000000   bar

    Relocation section '.rel.ARM.exidx' at offset 0x124 contains 1 entry:
    Offset     Info    Type            Sym.Value  Sym. Name
    00000000  0000022a R_ARM_PREL31      00000000   .text
    ```

-   The supported relocations are documented here:
    https://github.com/ARM-software/abi-aa/blob/576740d263827afdbb40ae1f01af6b76d4163b4b/aaelf32/aaelf32.rst#5612relocation-types

-   I found this line in binutils:
    ```none
    RD(SBREL32           , STATIC  , N, DATA , ((S + A) | T) - B(S)   ,  N, -1, N)
    ```

-   The problem seems to be, that it's not handled in the switch of `elf32_arm_final_link_relocate`

    -   I asked a question on StackOverflow go get some help:
        https://stackoverflow.com/questions/79870509/dangerous-relocation-unsupported-relocation-r-arm-sbrel32

    -   It seems that the `-fropi` isn't working either in `strerror`?

    -   I created a workaround where we essentially do the same thing the linker should do, just without a relocation.

-   There seems to be another problem with the `ElfEmbed` tool.
    It seems there was some undefined behavior in there that just worked before:

    -   `Generator::Generator` will call `StringTable::StringTable` to initialize `m_shstrtab`

    -   `StringTable::StringTable` then calls `Generator::create_section` which assumes that `m_shstrtab` is already initialized

    -   The solution would be to untangle this circular dependency somehow.
        This could be done by adding another overload for `Generator::create_section` that is called after the members are initialized

-   I resolved the circular dependency issue, but I immediatelly got the next issue:
    ```none
    ElfEmbed: /home/me/dev/pico-os/Tools/LibElf/MemoryStream.cpp:110: void Elf::MemoryStream::copy_to_raw_fd(int): Assertion `retval == size()' failed.
    ```
    After adding an exception handler, I've discovered the cause:
    ```none
    error: Invalid cross-device link
    ```
    It seems like `memfd_create` is not compatible with `copy_file_range`.
    It seems that this was a breaking change in the Linux kernel.

-   In theory, it should be possible to get everything working with GCC:
    `-msingle-pic-base`
    `-mpic-register=r9`
    `-mpic-data-is-text-relative`
    https://discourse.llvm.org/t/clang-and-fdpic-on-arm/87181
    This is not something I will attempt now, because it seems to work at the moment

-   I get another error when building the file system:
    ```none
    arm-none-eabi-objdump -d Kernel.1.elf >>Kernel.1.dis && cd /home/me/dev/pico-os/Build && /home/me/dev/pico-os/Build/elf2uf2/elf2uf2 Kernel.1.elf Kernel.1.uf2
    ld: Userland/FileSystem.elf: bad reloc symbol index (0x1201 >= 0xb) for offset 0x1db4 in section `.embed'
    ld: Userland/FileSystem.elf: error adding symbols: bad value
    collect2: error: ld returned 1 exit status
    ```
    Seems like some symbol is generated incorrectly?

    -   ChatGPT thinks that REL is not supported and that RELA needs to be used?
        However, I found documentation that contradicts that:
        https://github.com/ARM-software/abi-aa/blob/576740d263827afdbb40ae1f01af6b76d4163b4b/aaelf32/aaelf32.rst#561relocation-codes

    -   It seems that I did not initialize the variable that was used to write the index.
        Seems like a bug from the refactoring.

-   There are several other build errors that do not make any sense.
    How was I able to compile and run the application in this state?
    Maybe I messed something up during a merge?

-   There are several other linker issues:
    ```none
    ld: Userland/FileSystem.elf: bad reloc symbol index (0x8a690d >= 0xb) for offset 0x1db4 in section `.embed'
    ld: Userland/FileSystem.elf: error adding symbols: bad value
    ```

    -   This was caused by an uninitialized value that was used as the symbol offset

-   Now, it seems to work but I still get some linker warnings:
    ```none
    ld: libg.a(libc_a-closer.o): in function `_close_r': newlib/libc/reent/closer.c:47:(.text._close_r+0xc):
        warning: _close is not implemented and will always fail
    ld: libg.a(libc_a-closer.o):
        note: the message above does not take linker garbage collection into account
    ```

    -   This seems to be caused by a toolchain upgrade:
        https://stackoverflow.com/q/73742774/8746648

    -   This seems to be a bug that was fixed a while ago:
        https://github.com/raspberrypi/pico-sdk/issues/1029

-   I am trying to update the pico-sdk:

    -   Previously, I removed the weak `_exit` symbol, but the underlying code changed.
        I don't know how to migrate that change because I don't understand it.
        I will discard the change for now.

    -   The second tweak was to disable the `new` and `delete` definitions.
        However, this appears to be supported by this compiler flag now:
        `PICO_CXX_DISABLE_ALLOCATION_OVERRIDES=1`

    -   It seems that a similar option exists to make `printf` optional:
        `LIB_PICO_PRINTF_NONE`
        For now, I am skipping this commit, even though it may contain useful changes

-   Investigate the warning `arm-none-eabi-objcopy: Kernel.elf: section .heap lma 0x10023a30 adjusted to 0x100243a8`

    -   I thought this  happened because of my custom build SDK.
        However, that does not appear to be the case.

    -   I just removed the `objcopy`, not sure what I was doing there.

## Theories

## Tasks

-   Try to run on the actual kernel

-   Investigate how bad the alignment issue is.
    If necessary update my page allocator to remove alignment constraint.

## Delayed Tasks

-   Try to merge blocks together in `PageAllocator::deallocate_locked`

-   Try to avoid all remaining patches to Pico SDK

-   Add documentation that static variables are only partially supported

-   There is something called FDPIC in GCC which could be used to implement proper shared libraries?

-   Figure out if it's safe to use `LLD` instead?

## Answer (https://stackoverflow.com/q/79870509/8746648)

I suspect, that I was using a different linker before.  In my case it's detected automatically by CMake and it's possible that id made a different choice before.

If I use `arm-none-eabi-ld.gold` instead, it gives a different error message:
```none
$ arm-none-eabi-ld.gold ./stat.o -o Example.1.elf
./stat.o:stat.c:function baz:(.text+0xc): error: cannot relocate unimplemented reloc R_ARM_SBREL32 in object file
```

If I use `ld.lld` instead, it seems to work, but I am not sure if this is actually doing the correct thing, because it's not from the arm-none-eabi toolchain:
```none
$ ld.lld ./stat.o -o ./Example.1.elf
```
