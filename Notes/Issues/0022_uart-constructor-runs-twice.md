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

-   This forum threads points out that `openocd` rarely stops all the CPUs:

    https://forum.sparkfun.com/viewtopic.php?t=13066

    This causes the flashing to fail, and this is a known issue from 2008.
    I need to find some other tool.

-   It appears that there are no free alternatives for OpenOCD.

-   I could try to write my own OpenOCD alternative, but then I am in the same situation again.
    That tool will require using a debugger as well and then I need to use OpenOCD.

### Ideas

-   Exhaustively search for an alternative to OpenOCD in another issue.

-   How much does it take to replace OpenOCD and PicoProbe with a custom implementation.
    Maybe allocate some time for this and just try to implement it.

-   I could try using LLDB.

### Theories

-   Maybe the boot loader also sends CORE1 into `boot_3_vectors`, if they both run at exactly the same pace, it
    would be possible that we have a race condition.

-   Maybe the stack of CORE0 and CORE1 are identical and this causes issues before we are able to do the check.

-   Does OpenOCD have a cache, that might cause some confusion if that cache is global for both cores.
    Since the CPUID is at the same memory location, this would really break things.

### Conclusions
