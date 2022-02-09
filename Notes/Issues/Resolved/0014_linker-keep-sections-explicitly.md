commit e29dd47dae281ea22e1f7b341fa0654c1c398edf

To avoid issues in the future, we want to explicitly mention all sections in the linker script and provide an error if they are not mentioned.

### Notes

-   Currently, we fail, because the debug sections are modified when they are simply passed through:
    The following is produced by GDB when we load the executable:

    ```none
    Dwarf Error: bad offset (0x20001018) in compilation unit header (offset 0x0 + 6) [in module /home/me/dev/pico-os/Build/System.elf]
    ```

    Inspecting the executable reveals, that the addresses of the sections were changed:

    ```none
    $ readelf --sections Build/System.elf
    There are 20 section headers, starting at offset 0x21bc0:

    Section Headers:
    [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
    [ 0]                   NULL            00000000 000000 000000 00      0   0  0
    [ 1] .boot_2_flash     PROGBITS        20041f00 001f00 000100 00  AX  0   0  4
    [ 2] .text             PROGBITS        10000100 010100 000334 00  AX  0   0 16
    [ 3] .init_array       INIT_ARRAY      10000434 010434 000004 04  WA  0   0  4
    [ 4] .data             PROGBITS        20000000 020000 000000 00  WA  0   0  1
    [ 5] .bss              NOBITS          20000000 020000 000004 00  WA  0   0  1
    [ 6] .noinit           NOBITS          20000004 020000 000014 00  WA  0   0  4
    [ 7] .stack            NOBITS          20000018 020000 001000 00  WA  0   0  1
    [ 8] .ARM.attributes   ARM_ATTRIBUTES  20001018 020000 00002e 00      0   0  1
    [ 9] .comment          PROGBITS        20001018 02002e 00001e 01  MS  0   0  1
    [10] .debug_line       PROGBITS        20001018 02004c 0002d4 00      0   0  1
    [11] .debug_line_str   PROGBITS        20001018 020320 000099 01  MS  0   0  1
    [12] .debug_info       PROGBITS        20001018 0203b9 000459 00      0   0  1
    [13] .debug_abbrev     PROGBITS        20001018 020812 0001ce 00      0   0  1
    [14] .debug_aranges    PROGBITS        20001018 0209e0 000080 00      0   0  8
    [15] .debug_str        PROGBITS        20001018 020a60 000328 01  MS  0   0  1
    [16] .debug_frame      PROGBITS        20001018 020d88 000148 00      0   0  4
    [17] .symtab           SYMTAB          00000000 020ed0 0007e0 10     18  56  4
    [18] .strtab           STRTAB          00000000 0216b0 000443 00      0   0  1
    [19] .shstrtab         STRTAB          00000000 021af3 0000cb 00      0   0  1
    Key to Flags:
    W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
    L (link order), O (extra OS processing required), G (group), T (TLS),
    C (compressed), x (unknown), o (OS specific), E (exclude),
    y (purecode), p (processor specific)
    ```

    Compare this to before:

    ```none
    $ readelf --sections Build/System.elf
    There are 20 section headers, starting at offset 0x21ac0:

    Section Headers:
    [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
    [ 0]                   NULL            00000000 000000 000000 00      0   0  0
    [ 1] .boot_2_flash     PROGBITS        20041f00 001f00 000100 00  AX  0   0  4
    [ 2] .text             PROGBITS        10000100 010100 00030c 00  AX  0   0 16
    [ 3] .data             PROGBITS        20000000 020000 000000 00  WA  0   0  1
    [ 4] .init_array       INIT_ARRAY      20000000 020000 000004 04  WA  0   0  4
    [ 5] .bss              NOBITS          20000004 020004 000004 00  WA  0   0  1
    [ 6] .noinit           NOBITS          20000008 020004 000014 00  WA  0   0  4
    [ 7] .stack            NOBITS          2000001c 020004 001004 00  WA  0   0  1
    [ 8] .ARM.attributes   ARM_ATTRIBUTES  00000000 020004 00002e 00      0   0  1
    [ 9] .comment          PROGBITS        00000000 020032 00001e 01  MS  0   0  1
    [10] .debug_line       PROGBITS        00000000 020050 0002b2 00      0   0  1
    [11] .debug_line_str   PROGBITS        00000000 020302 000099 01  MS  0   0  1
    [12] .debug_info       PROGBITS        00000000 02039b 000404 00      0   0  1
    [13] .debug_abbrev     PROGBITS        00000000 02079f 0001c5 00      0   0  1
    [14] .debug_aranges    PROGBITS        00000000 020968 000080 00      0   0  8
    [15] .debug_str        PROGBITS        00000000 0209e8 0002e9 01  MS  0   0  1
    [16] .debug_frame      PROGBITS        00000000 020cd4 000144 00      0   0  4
    [17] .symtab           SYMTAB          00000000 020e18 0007c0 10     18  56  4
    [18] .strtab           STRTAB          00000000 0215d8 00041b 00      0   0  1
    [19] .shstrtab         STRTAB          00000000 0219f3 0000cb 00      0   0  1
    ```

-   After closer inspection of the linker script used by default, I was missing a zero that indicated that this section should be placed at address zero.

### Ideas

### Theories

### Conclusions

-   The linker script was changed from:

    ```none
    /* ... */
    .symtab : { *(.symtab) }
    /* ... */
    ```
    to:
    ```none
    /* ... */
    .symtab 0 : { *(.symtab) }
    /* ... */
    ```

    Now, the linker no longer modifies the sections.
