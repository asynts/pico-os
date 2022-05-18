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

-   We only start of with 32KiB out of 128KiB and then we use 16KiB for each process, if I understand things correctly.

-   I seem to recall, that the Pico SDK thing didn't use all the avaliable memory?

-   I was able to manually write to the blocks with the debugger, in other words, the memory is accessible.
    However, we still crash when we are setting `nullptr` to `Block::m_next` in `PageAllocater::PageAllocator`.

### Ideas

### Theories

-   I suspect, that we are creating so many threads, that we eventually run out of memory.

-   I suspect, that we try to allocate a huge chunk of memory which is simply too large to deal with.

-   I suspect, that the page allocator logic is broken somehow.

### Actions

-   Instead of trying to somehow fit my things into the heap, I did modify the linker script.
    Now, everything is put into the first 8 KiB of RAM and the rest can be managed by the page allocator.

-   After that, I ran into some trouble, because I was dereferencing a null pointer.
    Everything appears to be working now though.
