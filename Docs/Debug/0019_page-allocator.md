commitid 2a4c1abc66b839eb02ebca69988a38ad7acfcace

We appear to run out of memory in the page allocator.

### Notes

-   ```none
    > Editor.elf
    ASSERT(power < max_power)
    /home/me/dev/pico-os/Kernel/PageAllocator.cpp:0x00000057
    ```

-   ```none
    #0  Std::crash (format=0x10015f38 "ASSERT(%condition)\n%file:%line\n", condition=0x10016278 "power < max_power", file=0x10016128 "/home/me/dev/pico-os/Kernel/PageAllocator.cpp", line=87) at /home/me/dev/pico-os/Std/Forward.cpp:82
    #1  0x10008192 in Kernel::PageAllocator::allocate_locked (this=0x20001230 <Std::SingletonContainer<Kernel::PageAllocator>::m_instance>, power=18) at /home/me/dev/pico-os/Kernel/PageAllocator.cpp:87
    #2  0x100081a2 in Kernel::PageAllocator::allocate_locked (this=0x20001230 <Std::SingletonContainer<Kernel::PageAllocator>::m_instance>, power=17) at /home/me/dev/pico-os/Kernel/PageAllocator.cpp:89
    #3  0x100081a2 in Kernel::PageAllocator::allocate_locked (this=0x20001230 <Std::SingletonContainer<Kernel::PageAllocator>::m_instance>, power=16) at /home/me/dev/pico-os/Kernel/PageAllocator.cpp:89
    #4  0x100081a2 in Kernel::PageAllocator::allocate_locked (this=0x20001230 <Std::SingletonContainer<Kernel::PageAllocator>::m_instance>, power=15) at /home/me/dev/pico-os/Kernel/PageAllocator.cpp:89
    #5  0x100081a2 in Kernel::PageAllocator::allocate_locked (this=0x20001230 <Std::SingletonContainer<Kernel::PageAllocator>::m_instance>, power=14) at /home/me/dev/pico-os/Kernel/PageAllocator.cpp:89
    #6  0x100081a2 in Kernel::PageAllocator::allocate_locked (this=0x20001230 <Std::SingletonContainer<Kernel::PageAllocator>::m_instance>, power=13) at /home/me/dev/pico-os/Kernel/PageAllocator.cpp:89
    #7  0x100081a2 in Kernel::PageAllocator::allocate_locked (this=0x20001230 <Std::SingletonContainer<Kernel::PageAllocator>::m_instance>, power=12) at /home/me/dev/pico-os/Kernel/PageAllocator.cpp:89
    #8  0x100081a2 in Kernel::PageAllocator::allocate_locked (this=0x20001230 <Std::SingletonContainer<Kernel::PageAllocator>::m_instance>, power=11) at /home/me/dev/pico-os/Kernel/PageAllocator.cpp:89
    #9  0x1000805a in Kernel::PageAllocator::allocate (this=0x20001230 <Std::SingletonContainer<Kernel::PageAllocator>::m_instance>, power=11) at /home/me/dev/pico-os/Kernel/PageAllocator.cpp:58
    #10 0x1000bd5e in Kernel::Thread::setup_context<Kernel::SystemHandler::handle_next_waiting_thread()::<lambda()> >(struct {...} &&) (this=0x20010d88, callback=...) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:49
    #11 0x1000b87a in Kernel::SystemHandler::handle_next_waiting_thread (this=0x2000127c <Std::SingletonContainer<Kernel::SystemHandler>::m_instance>) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:48
    #12 0x1000b9ba in operator() (__closure=0x20016ffc) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:103
    #13 0x1000bf58 in operator() (__closure=0x20016ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:68
    #14 0x1000c27e in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::SystemHandler::SystemHandler()::<lambda()> >(Kernel::SystemHandler::SystemHandler()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::SystemHandler::SystemHandler()::<lambda()> >(Kernel::SystemHandler::SystemHandler()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x20016ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    ```

### Ideas

-   Verify that we are only scheduling on a single core.
