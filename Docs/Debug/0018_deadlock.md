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

### Ideas

-   Do we block in the kernel or in userspace?

-   How do we wake up a thread that is waiting to read with blocking?

-   Is the `sys$read` call ever made?

### Theories

-   The wakeup logic is broken and a thread keeps waiting for input and doesn't wake up.

### Actions
