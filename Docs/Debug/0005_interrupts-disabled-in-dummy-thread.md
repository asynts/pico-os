commitid 1e6e5ae5e674aa4032153bd07037ea1226b1e6ba

Somehow, we end up switching to the dummy thread with interrupts disabled.

### Notes

-   My understanding is, that the dummy thread should die immediately?

-   It seems, that we are trying to allocate from an interrupt, that can't go well.

    ```none
    #0  isr_hardfault () at /home/me/dev/pico-sdk/src/rp2_common/pico_standard_link/crt0.S:92
    #1  <signal handler called>
    #2  0x10010d2e in Std::MemoryAllocator::allocate (this=0x200012a4 <Std::SingletonContainer<Kernel::GlobalMemoryAllocator>::m_instance>, size=512, debug_override=true, address=0x10005a65 <Std::Vector<char, 256u>::ensure_capacity(unsigned int)+40>) at /home/me/dev/pico-os/Std/MemoryAllocator.cpp:29
    #3  0x100051e4 in Kernel::GlobalMemoryAllocator::allocate (this=0x200012a4 <Std::SingletonContainer<Kernel::GlobalMemoryAllocator>::m_instance>, size=512, debug_override=true, address=0x10005a65 <Std::Vector<char, 256u>::ensure_capacity(unsigned int)+40>) at /home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp:26
    #4  0x100052fa in operator new[] (size=512) at /home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp:67
    #5  0x10005a64 in Std::Vector<char, 256u>::ensure_capacity (this=0x20041e10, new_capacity=512) at /home/me/dev/pico-os/Std/Vector.hpp:71
    #6  0x10005394 in Std::Vector<char, 256u>::append (this=0x20041e10, value=@0x20041d43: 1 '\001') at /home/me/dev/pico-os/Std/Vector.hpp:39
    #7  0x1000cb02 in Std::StringBuilder::append (this=0x20041e10, value=...) at /home/me/dev/pico-os/Std/Format.hpp:169
    #8  0x100107be in Std::Formatter<Std::StringView>::format (builder=..., value=...) at /home/me/dev/pico-os/Std/Format.cpp:82
    #9  0x1000a798 in Std::VariadicFormatParams<Std::String, Kernel::Thread*>::VariadicFormatParams(Std::String const&, Kernel::Thread* const&)::{lambda(Std::StringBuilder&, void const*)#1}::operator()(Std::StringBuilder&, void const*) const (__closure=0x0, builder=..., value=0x200108b0) at /home/me/dev/pico-os/Std/Format.hpp:62
    #10 0x1000a7b6 in Std::VariadicFormatParams<Std::String, Kernel::Thread*>::VariadicFormatParams(Std::String const&, Kernel::Thread* const&)::{lambda(Std::StringBuilder&, void const*)#1}::_FUN(Std::StringBuilder&, void const*) () at /home/me/dev/pico-os/Std/Format.hpp:60
    #11 0x10004cfe in Std::vformat (builder=..., fmtstr=..., params=...) at /home/me/dev/pico-os/Std/Format.hpp:233
    #12 0x1000a008 in Std::dbgln<Std::String, Kernel::Thread*> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:250
    #13 0x10009692 in Std::dbgln<Std::String, Kernel::Thread*> (fmtstr=0x10016144 "[Thread::stash_context] m_name='{}' this={}") at /home/me/dev/pico-os/Std/Format.hpp:257
    #14 0x1000b4b4 in Kernel::Thread::stash_context (this=0x200108a8, context=...) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:88
    #15 0x1000c11e in Kernel::scheduler_next (context=0x20010698) at /home/me/dev/pico-os/Kernel/Threads/Scheduler.cpp:18
    #16 0x1000feac in isr_pendsv () at /home/me/dev/pico-os/Kernel/cpu.S:59
    ```

-   It seems we are allocating when we are trying to create the name of the thread to handle the system call:

    ```none
    #0  Std::crash (format=0x10015b84 "VERIFY(%condition)\n%file:%line\n", condition=0x1001618c "!Kernel::is_executing_in_handler_mode()", file=0x10016168 "/home/me/dev/pico-os/Std/Vector.hpp", line=74) at /home/me/dev/pico-os/Std/Forward.cpp:81
    #1  0x1000a68e in Std::Vector<Kernel::MPU::Region, 0u>::ensure_capacity (this=0x20010a44, new_capacity=1) at /home/me/dev/pico-os/Std/Vector.hpp:74
    #2  0x10009bea in Std::Vector<Kernel::MPU::Region, 0u>::append (this=0x20010a44, value=...) at /home/me/dev/pico-os/Std/Vector.hpp:52
    #3  0x1000d212 in Kernel::Thread::Thread (this=0x20010a24, name=...) at /home/me/dev/pico-os/Kernel/Threads/Thread.cpp:14
    #4  0x1000991c in Std::RefCounted<Kernel::Thread>::construct<Std::String> () at /home/me/dev/pico-os/Std/RefPtr.hpp:115
    #5  0x1000b8e8 in Kernel::syscall (context=0x20018ef8) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:29
    #6  0x100101a4 in isr_svcall () at /home/me/dev/pico-os/Kernel/cpu.S:78
    ```

-   After tweaking the assertions a bit:

    ```none
    #0  Std::crash (format=0x10014a28 "VERIFY(%condition)\n%file:%line\n", condition=0x10014abc "!Kernel::is_executing_in_handler_mode()", file=0x10014a84 "/home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp", line=23) at /home/me/dev/pico-os/Std/Forward.cpp:81
    #1  0x10005216 in Kernel::GlobalMemoryAllocator::allocate (this=0x200012a4 <Std::SingletonContainer<Kernel::GlobalMemoryAllocator>::m_instance>, size=67, debug_override=true, address=0x10000cb1 <Std::String::String(Std::StringView)+46>) at /home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp:23
    #2  0x1000535e in operator new[] (size=67) at /home/me/dev/pico-os/Kernel/GlobalMemoryAllocator.cpp:69
    #3  0x10000cb0 in Std::String::String (this=0x20041f6c, view=...) at /home/me/dev/pico-os/Std/Format.hpp:93
    #4  0x100089c8 in Std::StringBuilder::string (this=0x20041e30) at /home/me/dev/pico-os/Std/Format.hpp:185
    #5  0x1000ba68 in Std::String::format<Std::String, Kernel::Thread*, unsigned int> (fmtstr=...) at /home/me/dev/pico-os/Std/Format.hpp:197
    #6  0x1000b81e in Kernel::syscall (context=0x20018ef8) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:29
    #7  0x100100e8 in isr_svcall () at /home/me/dev/pico-os/Kernel/cpu.S:78
    ```

### Theory

-   I suspect, that the `KernelMutex` is keeping the thread alive somehow.

### Actions

-   I am no longer constructing the string in `dbgln` if we are in handler mode.
    That avoids the inevitiable memory allocation.
