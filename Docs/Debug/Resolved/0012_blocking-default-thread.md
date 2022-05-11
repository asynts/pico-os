commitid d3d02691235c419173085049b4b51af212a1b974

It seems that the default thread is blocking on a `KernelMutex`.

### Notes

-   When debugging, I saw that `m_holding_thread` was null which makes no sense.
    I was messing around with the debugger quite a bit, so this could be a coincidence.

-   It seems we have some sort of deadlock now.
    The first two system calls work and the system is running.

    However, when I press a key, the system crashes and I see the second `~Thread` message.

-   After adding the fallback thread, we are now crashing here:

    ```none
    #0  Std::crash (format=0x1001782c "VERIFY(%condition)\n%file:%line\n", condition=0x10017af0 "m_enabled", file=0x10017a5c "/home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp", line=104) at /home/me/dev/pico-os/Std/Forward.cpp:82
    #1  0x1000c5d8 in Kernel::Scheduler::trigger (this=0x20001328 <Std::SingletonContainer<Kernel::Scheduler>::m_instance>) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:104
    #2  0x10004f34 in Kernel::KernelMutex::lock (this=0x200013cc <Std::dbgln_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:47
    #3  0x100115b0 in Std::dbgln_raw (str=...) at /home/me/dev/pico-os/Std/Format.cpp:41
    #4  0x10007680 in Std::dbgln<>(Std::StringView) (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:268
    #5  0x1000738e in Std::dbgln<>(char const*) (fmtstr=0x10017550 "[SystemHandler] Before:") at /home/me/dev/pico-os/Std/Format.hpp:274
    #6  0x1000b574 in operator() (__closure=0x2001c7fc) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:54
    #7  0x1000bb10 in operator() (__closure=0x2001c7f8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:70
    #8  0x1000c118 in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::SystemHandler::handle_next_waiting_thread()::<lambda()> >(Kernel::SystemHandler::handle_next_waiting_thread()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::SystemHandler::handle_next_waiting_thread()::<lambda()> >(Kernel::SystemHandler::handle_next_waiting_thread()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x2001c7f8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    #9  0x00000000 in ?? ()
    ```

    My understanding is, that we masked `m_enabled` for something but then we use `Scheduler::trigger()` anyways.
    I think that was necessary to print out the state of the scheduler.

### Ideas

### Theories

### Actions

-   I added a fallback thread that can be scheduled if the default thread is blocked.

-   I ensured that `m_enabled` isn't disabled if we could block on a mutex.
