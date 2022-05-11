commitid 0e2a6c79a8c684fe28ac51314eb38a411bfb1752

We appear to have a deadlock.

### Notes

-   Adding a debug message in the dummy thread seemed to make a difference?

-   Now, we are hanging here:

    ```none
    #0  Kernel::setup_mpu (regions=...) at /home/me/dev/pico-os/Kernel/Loader.cpp:122
    #1  0x1000c562 in Kernel::Scheduler::schedule (this=0x20001328 <Std::SingletonContainer<Kernel::Scheduler>::m_instance>) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:97
    #2  0x1000c2ec in Kernel::scheduler_next (context=...) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:20
    #3  0x10010b60 in isr_pendsv () at /home/me/dev/pico-os/Kernel/cpu.S:59
    ```

    The first couple of characters were accepted but then we hang.
    I suspect, that this is some sort of deadlock and that we are looping in the `m_fallback_thread`.

    Here is the corresponding output:

    ```none
    [Scheduler] Dummy thread is running.
    [main] Creating /example.txt
    [Thread::setup_context::lambda] Thread 'Kernel (boot_with_scheduler)' is about to die.
    Loading executable for process '/bin/Shell.elf' from 0x100197ac
    Loaded executable:
      m_readonly_base=0x10019840
      m_writable_base=0x20018000
    GDB: add-symbol-file Userland/Shell.1.elf -s .text 0x10019850 -s .data 0x20018000 -s .bss 0x20018010
    [Process::create] argc=0x00000001 argv=0x2001a104 envp=0x2001a100
    [Process::create] argv:
      0x2001a10c: /bin/Shell.elf
    [Process::create] envp:
    [Process::create] ram_region.rbar=0x20018000
    [Process::create] rom_region.rbar=0x00000000
    Handing over execution to process '/bin/Shell.elf' at 0x1001a1c5
      Got argv=0x2001a104 and envp=0x2001a100
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    >[Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' is about to die.
    [Scheduler] Running default thread. (refcount=0x00000002)
    [Scheduler] We are about to kill thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' (refcount=0x00000001)
    [Thread::~Thread] m_name='Worker: '/bin/Shell.elf' (PID 0x00000000)'
    [Scheduler] Running default thread. (refcount=0x00000002)
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
     [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' is about to die.
    [Scheduler] Running default thread. (refcount=0x00000002)
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' is about to die.
    [Scheduler] We are about to kill thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' (refcount=0x00000001)
    [Thread::~Thread] m_name='Worker: '/bin/Shell.elf' (PID 0x00000000)'
    [Scheduler] Running default thread. (refcount=0x00000002)
    [Scheduler] We are about to kill thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' (refcount=0x00000001)
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    [Thread::~Thread] m_name='Worker: '/bin/Shell.elf' (PIeD 0x00000000)'
    [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' is about to die.
    ```

### Ideas

-   I should add `m_enabled` protection in other places maybe.

### Theories

### Actions
