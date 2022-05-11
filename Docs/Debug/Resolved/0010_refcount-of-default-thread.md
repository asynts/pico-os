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

-   We do have the correct reference count while the default thread runs.

-   I made `RefCounted::ref` virtual and put a breakpoint for the default thread:

    -   We start with `refcount()=1` when the object is created.

    -   ```none
        #0  Kernel::Thread::ref (this=0x20010178) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:125
        #1  0x1000553e in Std::RefPtr<Kernel::Thread>::operator= (this=0x200013c0 <Std::SingletonContainer<Kernel::Scheduler>::m_instance+152>, other=...) at /home/me/dev/pico-os/Std/RefPtr.hpp:74
        #2  0x1000c4d2 in Kernel::Scheduler::schedule (this=0x20001328 <Std::SingletonContainer<Kernel::Scheduler>::m_instance>) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:74
        #3  0x1000c31c in Kernel::scheduler_next (context=...) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:20
        #4  0x100109d0 in isr_pendsv () at /home/me/dev/pico-os/Kernel/cpu.S:59
        ```

    -   ```none
        #0  Kernel::Thread::unref (this=0x20010178) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:133
        #1  0x100053b8 in Std::RefPtr<Kernel::Thread>::clear (this=0x200013c0 <Std::SingletonContainer<Kernel::Scheduler>::m_instance+152>) at /home/me/dev/pico-os/Std/RefPtr.hpp:59
        #2  0x1000c476 in Kernel::Scheduler::schedule (this=0x20001328 <Std::SingletonContainer<Kernel::Scheduler>::m_instance>) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:63
        #3  0x1000c31c in Kernel::scheduler_next (context=...) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:20
        #4  0x100109d0 in isr_pendsv () at /home/me/dev/pico-os/Kernel/cpu.S:59
        ```

    -   ```none
        #0  Kernel::Thread::ref (this=0x20010178) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:125
        #1  0x1000553e in Std::RefPtr<Kernel::Thread>::operator= (this=0x200013c0 <Std::SingletonContainer<Kernel::Scheduler>::m_instance+152>, other=...) at /home/me/dev/pico-os/Std/RefPtr.hpp:74
        #2  0x1000c4d2 in Kernel::Scheduler::schedule (this=0x20001328 <Std::SingletonContainer<Kernel::Scheduler>::m_instance>) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:74
        #3  0x1000c31c in Kernel::scheduler_next (context=...) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:20
        #4  0x100109d0 in isr_pendsv () at /home/me/dev/pico-os/Kernel/cpu.S:59
        ```

    -   We do lock on the mutex after all, but why doesn't this cause a crash immediatelly?

        ```none
        #0  Kernel::Thread::ref (this=0x20010178) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:125
        #1  0x1000545e in Std::RefPtr<Kernel::Thread>::RefPtr (this=0x20015cbc, other=...) at /home/me/dev/pico-os/Std/RefPtr.hpp:27
        #2  0x10004ee4 in Kernel::KernelMutex::lock (this=0x200013c8 <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:43
        #3  0x10011294 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:41
        #4  0x1000771a in Std::dbgln<unsigned int> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
        #5  0x100073ba in Std::dbgln<unsigned int> (fmtstr=0x10017800 "[Scheduler] Running default thread. (refcount={})") at /home/me/dev/pico-os/Std/Format.hpp:274
        #6  0x1000c60e in operator() (__closure=0x20015ffc) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:118
        #7  0x1000ca48 in operator() (__closure=0x20015ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
        #8  0x1000cf8c in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x20015ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
        ```

    -   ```none
        #0  Kernel::Thread::unref (this=0x20010178) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:133
        #1  0x100053b8 in Std::RefPtr<Kernel::Thread>::clear (this=0x200013c8 <Std::dbgln_mutex>) at /home/me/dev/pico-os/Std/RefPtr.hpp:59
        #2  0x10004f74 in Kernel::KernelMutex::unlock (this=0x200013c8 <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:60
        #3  0x100112e2 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:46
        #4  0x1000771a in Std::dbgln<unsigned int> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
        #5  0x100073ba in Std::dbgln<unsigned int> (fmtstr=0x10017800 "[Scheduler] Running default thread. (refcount={})") at /home/me/dev/pico-os/Std/Format.hpp:274
        #6  0x1000c60e in operator() (__closure=0x20015ffc) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:118
        #7  0x1000ca48 in operator() (__closure=0x20015ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
        #8  0x1000cf8c in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x20015ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
        #9  0x00000000 in ?? ()
        ```

    -   ```none
        #0  Kernel::Thread::ref (this=0x20010178) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:125
        #1  0x1000545e in Std::RefPtr<Kernel::Thread>::RefPtr (this=0x20015cac, other=...) at /home/me/dev/pico-os/Std/RefPtr.hpp:27
        #2  0x10004ee4 in Kernel::KernelMutex::lock (this=0x200013c8 <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:43
        #3  0x10011294 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:41
        #4  0x1000cf40 in Std::dbgln<Std::ImmutableString, unsigned int> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
        #5  0x1000ca2e in Std::dbgln<Std::ImmutableString, unsigned int> (fmtstr=0x10017860 "[Scheduler] We are about to kill thread '{}' (refcount={})") at /home/me/dev/pico-os/Std/Format.hpp:274
        #6  0x1000c6dc in operator() (__closure=0x20015ffc) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:138
        #7  0x1000ca48 in operator() (__closure=0x20015ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
        #8  0x1000cf8c in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::Scheduler::loop()::<lambda()> >(Kernel::Scheduler::loop()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x20015ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
        #9  0x00000000 in ?? ()
        ```

-   We are holding the lock and not waiting for it, that causes the issue.

    In theory, this behaviour should be fine, I can remove the assertion.

-   We still crash though, if I understood the debugger correctly then we have `m_holding_thread == nullptr` which would suggest a race.
    However, I messed around with the debugger quite a bit, maybe that was incorrect.

### Ideas

-   I should trace every time that I am adding or removing a reference, maybe by making it virtual?

### Theories

-   I suspect, that the `RefPtr` implementation is broken somehow.

### Actions

-   The solution was to remove the incorrect assertion.
