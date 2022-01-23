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

### Ideas

-   I could add a symbol for `boot_1_reset`.

-   I could look at what `monitor reset` does.

### Theories

-   I think there is something weird with `monitor reset init` and that this is explained in `tcl/target/rp2040.cfg`.

### Conclusions
