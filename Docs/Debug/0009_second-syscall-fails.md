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

### Ideas

-   I should add the backup default thread just for safety.

    It can simply be choosen, when the default thread has `m_blocked=true`.

-   Debug print the state of the scheduler.

-   Am I adding the default thread into the scheduler list?

-   Print out the context before and after.

### Theories

-   I suspect, that the default thread blocks on the allocator and thus we don't have a default thread to schedule.

-   I suspect that the scheduler is in some fucked up state.

-   I suspect, that we save the context again and thus repeat the whole system call cycle in some
    fucked up way.
