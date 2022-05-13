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

-   I am looking at this:

    ```none
    [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' is about to die.
    [Scheduler] Running default thread. (refcount=0x00000002)
    [Scheduler] We are about to kill thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' (refcount=0x00000001)
    [Thread::~Thread] m_name='Worker: '/bin/Shell.elf' (PID 0x00000000)'
    [Scheduler] Running default thread. (refcount=0x00000002)
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
     [Thread::setup_context::lambda] Thread 'Worker: '/bin/Shell.elf' (PID 0x00000000)' is about to die.
    [Scheduler] Running default thread. (refcount=0x00000002)
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    ```

    The key thing here is, that the second `~Thread` call is missing.

    If I then press another key, we do see that call.
    I suspect that we are locking there for the debug output, however, it is unclear who else could be holding it.

-   I am using the following commands to automatically obtain a backtrace at each call to `KernelMutex`:

    ```none
    # Configure GDB to create a log file.
    set logging on

    # First, we need to get to the relevant part of the program.
    rebuild
    break Process.cpp:116
    run
    delete

    # Now, we set breakpoints on KernelMutex.
    break KernelMutex.hpp:32
    command
      backtrace
      continue
    end
    break KernelMutex.hpp:57
    command
      backtrace
      continue
    end

    # Let's go.
    ```

-   This is the result that I got from tracing:

    ```none
    Thread 1 hit Breakpoint 1, operator() (__closure=0x200177cc) at /home/me/dev/pico-os/Kernel/Process.cpp:116
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    ```

-   If we split that up based on the mutex:

    ```none
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    ```

    ```none
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001470 <Kernel::page_allocator_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    ```

    ```none
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x20001424 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    ```

-   We can clearly tell that the `Std::dbgln_mutex` has some contention.
    The rest looks fine.

    Here are the last six relevant backtraces:

    ```none
    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    32	            VERIFY(Kernel::is_executing_in_thread_mode());
    #0  Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    #1  0x10011424 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:41
    #2  0x1000d100 in Std::dbgln<Std::ImmutableString, unsigned int> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
    #3  0x1000ca26 in Std::dbgln<Std::ImmutableString, unsigned int> (fmtstr=0x100179c0 "[Scheduler] We are about to kill thread '{}' (refcount={})") at /home/me/dev/pico-os/Std/Format.hpp:274
    #4  0x1000c6dc in operator() (__closure=0x20015ffc) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:139
    #5  0x1000ca40 in operator() (__closure=0x20015ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
    #6  0x1000d14c in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x20015ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    #7  0x00000000 in ?? ()
    target halted due to debug-request, current mode: Thread
    xPSR: 0x01000000 pc: 0x0000012a msp: 0x20041f00

    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    32	            VERIFY(Kernel::is_executing_in_thread_mode());
    #0  Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    #1  0x10011424 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:41
    #2  0x1000c02a in Std::dbgln<Std::ImmutableString> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
    #3  0x1000b93e in Std::dbgln<Std::ImmutableString> (fmtstr=0x100173c4 "[SystemHandler] Dealing with system call for '{}'") at /home/me/dev/pico-os/Std/Format.hpp:274
    #4  0x1000b604 in Kernel::SystemHandler::handle_next_waiting_thread (this=0x20001284 <Std::SingletonContainer<Kernel::SystemHandler>::m_instance>) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:29
    #5  0x1000b7c2 in operator() (__closure=0x200157fc) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:92
    #6  0x1000bd78 in operator() (__closure=0x200157f8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
    #7  0x1000c136 in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::SystemHandler::SystemHandler()::<lambda()> >(Kernel::SystemHandler::SystemHandler()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::SystemHandler::SystemHandler()::<lambda()> >(Kernel::SystemHandler::SystemHandler()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x200157f8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    #8  0x00000000 in ?? ()
    target halted due to debug-request, current mode: Thread
    xPSR: 0x01000000 pc: 0x0000012a msp: 0x20041f00

    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    57	            VERIFY(Kernel::is_executing_in_thread_mode());
    #0  Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    #1  0x10011472 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:46
    #2  0x1000d100 in Std::dbgln<Std::ImmutableString, unsigned int> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
    #3  0x1000ca26 in Std::dbgln<Std::ImmutableString, unsigned int> (fmtstr=0x100179c0 "[Scheduler] We are about to kill thread '{}' (refcount={})") at /home/me/dev/pico-os/Std/Format.hpp:274
    #4  0x1000c6dc in operator() (__closure=0x20015ffc) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:139
    #5  0x1000ca40 in operator() (__closure=0x20015ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
    #6  0x1000d14c in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x20015ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    #7  0x00000000 in ?? ()
    target halted due to debug-request, current mode: Thread
    xPSR: 0x01000000 pc: 0x0000012a msp: 0x20041f00

    Thread 1 hit Breakpoint 2, Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    32	            VERIFY(Kernel::is_executing_in_thread_mode());
    #0  Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:32
    #1  0x10011424 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:41
    #2  0x1000c02a in Std::dbgln<Std::ImmutableString> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
    #3  0x1000b93e in Std::dbgln<Std::ImmutableString> (fmtstr=0x10017ed4 "[Thread::~Thread] m_name='{}'") at /home/me/dev/pico-os/Std/Format.hpp:274
    #4  0x1000d952 in Kernel::Thread::~Thread (this=0x20010c88, __in_chrg=<optimized out>) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:43
    #5  0x1000d9b0 in Kernel::Thread::~Thread (this=0x20010c88, __in_chrg=<optimized out>) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:44
    #6  0x1000f3ce in Std::RefCounted<Kernel::Thread>::unref (this=0x20010c88) at /home/me/dev/pico-os/Std/RefPtr.hpp:144
    #7  0x100053b8 in Std::RefPtr<Kernel::Thread>::clear (this=0x20015fb8) at /home/me/dev/pico-os/Std/RefPtr.hpp:59
    #8  0x1000c708 in operator() (__closure=0x20015ffc) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:143
    #9  0x1000ca40 in operator() (__closure=0x20015ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
    #10 0x1000d14c in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x20015ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    #11 0x00000000 in ?? ()
    target halted due to debug-request, current mode: Thread
    xPSR: 0x01000000 pc: 0x0000012a msp: 0x20041f00

    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    57	            VERIFY(Kernel::is_executing_in_thread_mode());
    #0  Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    #1  0x10011472 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:46
    #2  0x1000c02a in Std::dbgln<Std::ImmutableString> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
    #3  0x1000b93e in Std::dbgln<Std::ImmutableString> (fmtstr=0x100173c4 "[SystemHandler] Dealing with system call for '{}'") at /home/me/dev/pico-os/Std/Format.hpp:274
    #4  0x1000b604 in Kernel::SystemHandler::handle_next_waiting_thread (this=0x20001284 <Std::SingletonContainer<Kernel::SystemHandler>::m_instance>) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:29
    #5  0x1000b7c2 in operator() (__closure=0x200157fc) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:92
    #6  0x1000bd78 in operator() (__closure=0x200157f8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
    #7  0x1000c136 in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::SystemHandler::SystemHandler()::<lambda()> >(Kernel::SystemHandler::SystemHandler()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::SystemHandler::SystemHandler()::<lambda()> >(Kernel::SystemHandler::SystemHandler()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x200157f8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    #8  0x00000000 in ?? ()
    target halted due to debug-request, current mode: Thread
    xPSR: 0x01000000 pc: 0x0000012a msp: 0x20041f00

    Thread 1 hit Breakpoint 3, Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    57	            VERIFY(Kernel::is_executing_in_thread_mode());
    #0  Kernel::KernelMutex::unlock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:57
    #1  0x10011472 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:46
    #2  0x1000c02a in Std::dbgln<Std::ImmutableString> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
    #3  0x1000b93e in Std::dbgln<Std::ImmutableString> (fmtstr=0x10017ed4 "[Thread::~Thread] m_name='{}'") at /home/me/dev/pico-os/Std/Format.hpp:274
    #4  0x1000d952 in Kernel::Thread::~Thread (this=0x20010c88, __in_chrg=<optimized out>) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:43
    #5  0x1000d9b0 in Kernel::Thread::~Thread (this=0x20010c88, __in_chrg=<optimized out>) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:44
    #6  0x1000f3ce in Std::RefCounted<Kernel::Thread>::unref (this=0x20010c88) at /home/me/dev/pico-os/Std/RefPtr.hpp:144
    #7  0x100053b8 in Std::RefPtr<Kernel::Thread>::clear (this=0x20015fb8) at /home/me/dev/pico-os/Std/RefPtr.hpp:59
    #8  0x1000c708 in operator() (__closure=0x20015ffc) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:143
    #9  0x1000ca40 in operator() (__closure=0x20015ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
    #10 0x1000d14c in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x20015ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    #11 0x00000000 in ?? ()
    target halted due to debug-request, current mode: Thread
    xPSR: 0x01000000 pc: 0x0000012a msp: 0x20041f00
    ```

-   It seems that `Kernel::SystemHandler::handle_next_waiting_thread` is fighting with `Kernel::Thread::~Thread`.
    The latter seems to come from the default thread.

### Ideas

-   I should add `m_enabled` protection in other places maybe.

-   I should add an assertion that checks that if we schedule away from something, it can't be holding any locks.

-   I should use the builtin mutex things from the processor.

### Theories

-   It seems that `Kernel::SystemHandler::handle_next_waiting_thread` isn't able to be scheduled while it holds a lock.

-   Could this be a race condition in the `KernelMutex` implementation?
    Is that thing even thread safe?

### Actions
