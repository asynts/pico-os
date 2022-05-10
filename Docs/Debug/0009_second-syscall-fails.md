commitid 2bc7b770051687bf90d06e9052901383d5d548ed

Making a single system call appears to work, however, the second call raises an assertion.

### Notes

-   The first system call appears to be a `sys$write` for the `>` character for the shell.
    This character appears correctly.

    The second system call should be another `sys$write` for the ` `.
    This character does not appear.

### Ideas

-   Debug print the state of the scheduler.

### Theories

-   I suspect that the scheduler is in some fucked up state.
