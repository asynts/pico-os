commitid c289fc62fcea89008172d7d523b9199cab63e5be

We appear to deadlock after using `sys$write`.

### Notes

-   This could be the deadlock issue that I encountered before.

-   We do get the `> ` output, but after that we simply freeze.
    Pressing keys doesn't appear to do anything.

-   It seems that the output of `sys$write` and the debug output are interleaved.
    That suggests a lack of synchronization:

    ```none
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    [Thread::~Thread] m_name='Worke r: '/bin/Shell.elf' (PID 0x00000000)'
    [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' is about to die.
    ```

    Notice the `Worke r`.

-   There seems to be some race, another time I was able to get the `> ` without interference, and then, the next system call worked.

-   It seems, we block when cleaning up.

    ```none
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    Thread::sys$read
    [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000, SYSCALL 0x00000001)' is about to die.
    ```

    We are stuck with this backtrace:

    ```none
    #0  0x1000720c in Kernel::setup_mpu (regions=...) at /home/me/dev/pico-os/Kernel/Loader.cpp:112
    #1  0x1000c92e in Kernel::Scheduler::schedule (this=0x20001328 <Std::SingletonContainer<Kernel::Scheduler>::m_instance>) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:100
    #2  0x1000c6a0 in Kernel::scheduler_next (context=...) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:21
    #3  0x10010f5c in isr_pendsv () at /home/me/dev/pico-os/Kernel/cpu.S:59
    ```

    Seems there is nothing sensible to schedule.

-   In `ConsoleDevice.cpp`, we loop indefinitively until we are able to read something.
    Maybe, we get stuck there?

-   I did manage to reproduce this with `sys$write`, this is clearly a race and probably closely related with `dbgln`.

    ```none
    Scheduling old thread again.
    Scheduling old thread again. (DONE)
    [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000, SYSCALL 0x00000002)' is about to die.
    ```

    We never see the `Thread::~Thread` message for that, we block before that happens.

-   Actually, the message is produced from `Thread::die` and not from the default thread like I originally thought.
    For some reason, we do not schedule the default thread then.

### Ideas

### Theories

-   I suspect, that the scheduling logic is broken and that we do not schedule the default thread when we should.
    Maybe there is another thread always around that...

    Oops, the thread reading input is always around.
    It needs to block, otherwise the default thread might not get a turn if it blocked.

    That would make this a livelock.

-   I suspect, that I am allocating in an interrupt handler somewhere.

### Actions

-   Added interrupt guards to `UART`.

-   I removed the `KernelMutex` usage from `dbgln`.

    In the future, I need to figure this out, but for now, I can just mask interrupts and write to UART.
