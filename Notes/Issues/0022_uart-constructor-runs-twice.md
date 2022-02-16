commit 4289d970c871e331a260519428d74a4da99263e9

Somehow, the constructor from `UartOutputDriver` is executed twice.

### Notes

-   When I set a breakpoint at `UartOutputDriver::UartOutputDriver()`, this breakpoint is hit twice.

-   In theory, the `Singleton<UartOutputDriver>::m_initialized` should prevent this from happening.
    There are no two calls in the code either.

-   I was trying to only run one of the threads and hold the other in with the debugger.
    However, it seems that GDB changes the thread IDs on `run`.
    I also received the following error which suggests, that GDB is configured incorrectly somehow:

    ```none
    (gdb) set scheduler-locking on
    (gdb) run
    Starting program: /home/me/dev/pico-os/Build/System.elf
    warning: while parsing target memory map (at line 2): Required element <memory> is missing
    ```

    When I tried to manually switch to the other thread, GDB crashed:

    ```none
    (gdb) thread 1
    (gdb) continue
    ../../gdb/inline-frame.c:384: internal-error: void skip_inline_frames(thread_info*, bpstat): Assertion `find_inline_frame_state (thread) == NULL' failed.
    ```

-   It does seem like the boot loader hands over for both CORE0 and CORE1, however, I can not distinguish the two.
    Somehow neither of them has `SIO.CPUID=1` which makes no sense.

    It appears that the debugger is completely confused.
    When ever I continue it changes the thread IDs around and sometimes it crashes when it tries to generate a backtrace.
    I don't know how to continue.

### Ideas

-   Try adding `volatile` to `m_initialized`.

-   Read the documentation about the boot loader.

-   I could try using LLDB.

### Theories

-   Maybe the boot loader also sends CORE1 into `boot_3_vectors`, if they both run at exactly the same pace, it
    would be possible that we have a race condition.

### Conclusions
