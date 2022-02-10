commit 2ba0c50ce953b4988f1b65e5e5fad4ee7a4a5680

Somehow, somewhere something is put into the `.init_array` section.

### Notes

-   After removing `export module boot` from `boot_4_load_kernel.cpp`, this section is gone.
    It is still unclear, why the compiler generates this in the first place and what it does though.

-   It is unclear what this constructor does, it is called `_ZGIW4bootEv` which is not a valid mangled name.

-   This is the disassembly of the symbol:

    ```none
    000001f4 <_ZGIW4bootEv>:
    1f4:	b580      	push	{r7, lr}
    1f6:	af00      	add	r7, sp, #0
    1f8:	4b04      	ldr	r3, [pc, #16]	; (20c <_ZGIW4bootEv+0x18>)
    1fa:	781b      	ldrb	r3, [r3, #0]
    1fc:	2b00      	cmp	r3, #0
    1fe:	d102      	bne.n	206 <_ZGIW4bootEv+0x12>
    200:	4b02      	ldr	r3, [pc, #8]	; (20c <_ZGIW4bootEv+0x18>)
    202:	2201      	movs	r2, #1
    204:	701a      	strb	r2, [r3, #0]
    206:	46bd      	mov	sp, r7
    208:	bd80      	pop	{r7, pc}
    20a:	46c0      	nop			; (mov r8, r8)
    20c:	00000000 	.word	0x00000000
    ```

    I would decompile this code as follows:

    ```c
    // This is called '.bss' in assembly.
    extern unsigned char bss[];

    __attribute__((constructor))
    void boot() {
        if (bss[0] == 0) {
            bss[0] = 1;
        }
    }
    ```

-   The object file has a `.bss` section with exactly one byte which is the flag we set during init.
    Seems harmless.

### Ideas

### Theories

### Conclusions

-   It appears that the C++20 module stuff adds a flag that keeps track when a module is initialized.
    Not sure what this is useful for.
