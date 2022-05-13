commitid 11615ec31283da624383387370686395068d9e7b

We aquire a `KernelMutex` during the boot process, that seems dangerous.

### Notes

-   We hit an assertion, because interrupts aren't enabled but we aquire a mutex.

-   We enter `boot` with interrupts enabled and in thread mode.
    That's not what I expected.

-   Now, we are crashing here, not sure if this is even relevant to this ticket anymore.

    ```none
    #0  Std::crash (format=0x10015c58 "VERIFY(%condition)\n%file:%line\n", condition=0x10015c48 "were_enabled", file=0x10015c1c "/home/me/dev/pico-os/Kernel/HandlerMode.hpp", line=83) at /home/me/dev/pico-os/Std/Forward.cpp:82
    #1  0x10004dba in Kernel::MaskedInterruptGuard::MaskedInterruptGuard (this=0x20014f10) at /home/me/dev/pico-os/Kernel/HandlerMode.hpp:83
    #2  0x10004fea in Kernel::KernelMutex::lock (this=0x20001428 <Kernel::malloc_mutex>) at /home/me/dev/pico-os/Kernel/KernelMutex.hpp:40
    #3  0x100052dc in Kernel::GlobalMemoryAllocator::allocate (this=0x200012f0 <Std::SingletonContainer<Kernel::GlobalMemoryAllocator>::m_instance>, size=40, debug_override=true, address=0x10003611 <Kernel::FlashFileSystem::FlashFileSystem()+28>) at /home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp:31
    #4  0x100053ea in operator new (size=40) at /home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp:69
    #5  0x10003610 in Kernel::FlashFileSystem::FlashFileSystem (this=0x200012d0 <Std::SingletonContainer<Kernel::FlashFileSystem>::m_instance>) at /home/me/dev/pico-os/Kernel/FileSystem/FlashFileSystem.cpp:13
    #6  0x100114ee in Std::Singleton<Kernel::FlashFileSystem>::initialize<>() () at /home/me/dev/pico-os/Std/Singleton.hpp:32
    #7  0x10010ff2 in Kernel::boot_with_scheduler () at /home/me/dev/pico-os/Kernel/main.cpp:59
    #8  0x10011284 in Kernel::Thread::setup_context<void (&)()>(void (&)())::{lambda()#1}::operator()() (__closure=0x20014ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:68
    #9  0x1001163e in type_erased_member_function_wrapper<Kernel::Thread::setup_context<void (&)()>(void (&)())::<lambda()>, &Kernel::Thread::setup_context<void (&)()>(void (&)())::<lambda()>::operator()>(void *) (object=0x20014ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    #10 0x00000000 in ?? ()
    ```

### Ideas

### Actions

-   We are now creating the `SystemHandler` thread later, when the scheduler is already running.

-   I made it possible to disable a `KernelMutex` during startup.
