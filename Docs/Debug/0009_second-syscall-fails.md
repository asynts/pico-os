commitid 2bc7b770051687bf90d06e9052901383d5d548ed

Making a single system call appears to work, however, the second call raises an assertion.

### Notes

-   The first system call appears to be a `sys$write` for the `>` character for the shell.
    This character appears correctly.

    The second system call should be another `sys$write` for the ` `.
    This character does not appear.

-   It seems, that the default thread already has a stashed context and we try to override it.

-   The issue occurs shortly after we add the calling thread back to the scheduler.

-   I tried to print out the scheduler state but it seems that there are two things that collided:

    ```none
    >[SystemHandler] Before:
    [Scheduler] m_queued_threads:
    [Scheduler] m_danging_threads:
    [Scheduler] m_default_thread:
    Default Thread (Core 0) @0x20010178
    [SystemHandler] After:
    [Scheduler] m_queued_threads:
    Kernel: SystemHandler @0x200100c4
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
     [SystemHandler] Before:
    [Scheduler] m_queued_threads:
    Worker: '/bin/Shell.elf' (PID 0x00000000) @0x20010bd4
    [Scheduler] m_danging_threads:
    [Scheduler] m_danging_threads:
    [Scheduler] m_default_thread:
    [Scheduler] m_default_thread:
    Default Thread (Core 0) @0x20010178
    Default Thread (Core 0) @0x20010178
    [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' (0x20010bd4) returned
    ```

    This doesn't necessarily mean that there was a collision, however, it does suggest some sort of race.

    Notice that there is a space before `[SystemHandler] Before:`.
    In other words, the system call was executed correctly.

-   I think, that the `dbgln` took so long that a `SysTick` and `PendSV` happend.
    After temporarily disabling the scheduler, I got the expected result:

    ```none
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    >[SystemHandler] Before:
    [Scheduler] m_queued_threads:
    [Scheduler] m_danging_threads:
    [Scheduler] m_default_thread:
    Default Thread (Core 0) @0x20010178
    [SystemHandler] After:
    [Scheduler] m_queued_threads:
    Process: /bin/Shell.elf @0x20010acc
    [Scheduler] m_danging_threads:
    [Scheduler] m_default_thread:
    Default Thread (Core 0) @0x20010178
    [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' (0x20010bd4) returned
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    ```

-   Somewhere, we are calling `Kernel::enable_interrupts` and then we are immediately in `PendSV`.
    We can tell by inspecting the `context` passed to `scheduler_next`.

    However, the rest of the context doesn't really make any sense.

-   I was not able to stop at a breakpoint in the default thread, it seems, that it never runs and it doesn't produce any output either.

-   If I never return from the worker thread, then the crash doesn't occur and the output looks correct:

    ```none
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    >[SystemHandler] Before:
    [Scheduler] m_queued_threads:
    [Scheduler] m_danging_threads:
    [Scheduler] m_default_thread:
      Default Thread (Core 0) @0x20010178
    [SystemHandler] After:
    [Scheduler] m_queued_threads:
      Process: /bin/Shell.elf @0x20010acc
    [Scheduler] m_danging_threads:
    [Scheduler] m_default_thread:
      Default Thread (Core 0) @0x20010178
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
     [SystemHandler] Before:
    [Scheduler] m_queued_threads:
      Worker: '/bin/Shell.elf' (PID 0x00000000) @0x20010bd4
    [Scheduler] m_danging_threads:
    [Scheduler] m_default_thread:
      Default Thread (Core 0) @0x20010178
    [SystemHandler] After:
    [Scheduler] m_queued_threads:
      Worker: '/bin/Shell.elf' (PID 0x00000000) @0x20010bd4
      Process: /bin/Shell.elf @0x20010acc
    [Scheduler] m_danging_threads:
    [Scheduler] m_default_thread:
      Default Thread (Core 0) @0x20010178
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    ```

    Notice that the worker thread still remains queued.

-   I tried setting up some deadlock protection logic, however, this did not help, we do not appear to deadlock,
    the default thread never runs.

### Ideas

-   Print out the context before and after.

-   Rewrite the scheduler logic.

### Theories

-   I suspect, that the scheduler logic is broken after the recent changes.

-   I suspect, that we forget to update the active thread somewhere.

-   I suspect, that we save the context again and thus repeat the whole system call cycle in some
    fucked up way.
