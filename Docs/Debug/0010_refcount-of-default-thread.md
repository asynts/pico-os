commitid 50ed7a4978bef434cb8db00ff08f77a0bb127ef4

We hit an assertion because the default thread doesn't have `refcount() == 2` when it should.

### Notes

-   ```none
    [SystemHandler] Before:
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
    [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' is about to die.
    [Scheduler] Running default thread.
    [Thread::~Thread] m_name='Worker: '/biVERIFY(m_active_thread->refcount() == 2)
    /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:0x0000003e
    ```

### Ideas

### Theories

### Actions
