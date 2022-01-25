commit 18ec8c81ec21ce5511ffb2103a645b38f4576344

When I do `run` in GDB, I would expect, that the chip is reset and then goes through the startup routine.

### Notes

-   In GDB anything passed to `monitor` is forwarded to the remote, in this case `openocd`.
    The commands are defined in `target.c`:

    https://github.com/raspberrypi/openocd/blob/df76ec7edee9ebb9be86e4cff7479da642b0e8df/src/target/target.c#L5478

-   The reset command is declared here:

    ```c
    {
		.name = "reset",
		.handler = handle_reset_command,
		.mode = COMMAND_EXEC,
		.usage = "[run|halt|init]",
		.help = "Reset all targets into the specified mode. "
			"Default reset mode is run, if not given.",
	},
    ```
    https://github.com/raspberrypi/openocd/blob/df76ec7edee9ebb9be86e4cff7479da642b0e8df/src/target/target.c#L6514-L6521

    and is defined here:

    ```c
    COMMAND_HANDLER(handle_reset_command)
    {
        if (CMD_ARGC > 1)
            return ERROR_COMMAND_SYNTAX_ERROR;

        enum target_reset_mode reset_mode = RESET_RUN;
        if (CMD_ARGC == 1) {
            const Jim_Nvp *n;
            n = Jim_Nvp_name2value_simple(nvp_reset_modes, CMD_ARGV[0]);
            if ((n->name == NULL) || (n->value == RESET_UNKNOWN))
                return ERROR_COMMAND_SYNTAX_ERROR;
            reset_mode = n->value;
        }

        /* reset *all* targets */
        return target_process_reset(CMD, reset_mode);
    }
    ```
    https://github.com/raspberrypi/openocd/blob/df76ec7edee9ebb9be86e4cff7479da642b0e8df/src/target/target.c#L3301-L3317

    the enum values are defined here:

    ```c
    enum target_reset_mode {
        RESET_UNKNOWN = 0,
        RESET_RUN = 1,		/* reset and let target run */
        RESET_HALT = 2,		/* reset and halt target out of reset */
        RESET_INIT = 3,		/* reset and halt target out of reset, then run init script */
    };
    ```
    https://github.com/raspberrypi/openocd/blob/df76ec7edee9ebb9be86e4cff7479da642b0e8df/src/target/target.h#L74-L79

-   It appears that most of the magic occurs in a tcl script:

    https://github.com/raspberrypi/openocd/blob/df76ec7edee9ebb9be86e4cff7479da642b0e8df/tcl/target/rp2040.cfg

-   I am able to set a breakpoint in the bootrom as follows:

    ```none
    > monitor reset init
    > load
    > x/wx 0x00000004
    0x000000ef
    ```

    This is the address at with we will enter upon reset.
    We set a hardware assisted breakpoint there:

    ```none
    > monitor reset init
    > load
    > hbreak *0x000000ef
    > run
    ```

    However, when I continue, I hit another breakpoint.
    This may be an assertion, because the checksum is incorrect.

-   It is possible to extact a single section from an ELF file using:

    ```none
    arm-none-eabi-objcopy --dump-section .boot2=boot2.bin hello_serial.elf
    ```

-   I manually computed the checksum of `hello_serial.elf` and it matched to the original.

-   I tried manually changing the checksum of `hello_serial.elf` to see what happens.

    Unmodified:
    ```none
    file hello_serial.elf
    break main
    load
    continue
    Thread 1 hit Breakpoint 1, 0x100002e6 in main ()
    ```

    Modified:
    ```none
    file hello_serial.patched.elf
    break main
    load
    continue
    ```

    If I manually interrupt we are at address `0x00001bd0`, if I interrupt very quickly after only a few tenths
    of a second, we are at address `0x00002850`.

    My understanding is, that we retry for 0.5 seconds and then give up.
    I suspect that the first address belongs to the wait statement of the retry logic and the other belongs to
    the logic that comes after.

    If we do the same thing with my logic, it works both times.
    This is because we are running from a debugger.
    My conclusion is, that the other bootrom seems to detect this situation and runs the reset logic manually?

    If I use `run` instead of `continue`, we appear to reset the chip which is what should happen in my opinion.
    We reach the same address `0x00001bd0` after a while, but I am unable to trigger the other case.
    Although, I've managed to stop it at `0x00001d84` and `0x00001d60`.
    I suspect, that the `run` has a slight delay which makes it hard to stop the chip fast enough.

-   If I `run` my system with the correct checksum, I hit the breakpoint at `0x20041f02` and disassembly reveals,
    that my boot program was loaded at address `0x20041f00` instead of the expected `0x20000000`:

    ```none
       0x20041efe:	movs	r0, r0
       0x20041f00:	movs	r1, #13
    => 0x20041f02:	bkpt	0x0000
       0x20041f04:	wfi
       0x20041f06:	b.n	0x20041f00
       0x20041f08:	movs	r0, r0
       0x20041f0a:	movs	r0, r0
    ```

-   It turns out that we are loaded at the top of SRAM5 which means `0x20041f00`.
    https://github.com/raspberrypi/pico-sdk/blob/2062372d203b372849d573f252cf7c6dc2800c0a/src/rp2_common/boot_stage2/boot_stage2.ld

### Ideas

-   Do the breakpoints persist when the chip is reset?
    Does GDB get confused with this?

-   Can I ask the chip which breakpoints it knows?

### Theories

### Conclusions

-   When we use `run` we really reset the chip and go through the bootrom.
    It appears that it doesn't load our flash stage at address `0x20000000` though.
    Instead, it is loaded at address `0x20041f00`, this is the top of SRAM5.
