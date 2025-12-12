commitid a0049d081678b4376cd65d12b9fb8ad3156725bd

### Notes

-   Currently, the system handler allocates in order to create a `RefPtr<Thread>` which is used to do the actual work.

    ```none
    #0  Std::crash (format=0x100146f0 "VERIFY(%condition)\n%file:%line\n", condition=0x10014710 "Kernel::is_executing_in_thread_mode()", file=0x10014774 "/home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp", line=23) at /home/me/dev/pico-os/Std/Forward.cpp:82
    #1  0x10005368 in Kernel::GlobalMemoryAllocator::allocate (this=0x200012a8 <Std::SingletonContainer<Kernel::GlobalMemoryAllocator>::m_instance>, size=60, debug_override=true, address=0x1000b905 <Std::RefCounted<Kernel::Thread>::construct<Std::String&>(Std::String&)+16>) at /home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp:23
    #2  0x1000548a in operator new (size=60) at /home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp:64
    #3  0x1000b904 in Std::RefCounted<Kernel::Thread>::construct<Std::String&> () at /home/me/dev/pico-os/Std/RefPtr.hpp:114
    #4  0x1000b792 in Kernel::syscall (context=0x20018ef8) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:29
    #5  0x1000f9f4 in isr_svcall () at /home/me/dev/pico-os/Kernel/cpu.S:78
    ```

-   I had some difficulties, convincing the system handler to block until more requests come through.
    Currently, I have that code commented out.

### Ideas

-   I could add an `m_requested_system_call` flag and then handle that flag in another thread.
