commitid c289fc62fcea89008172d7d523b9199cab63e5be

We crash when making a system call.

### Notes

-   ```none
    [SystemHandler] Dealing with system call for 'Process: /bin/Shell.elf'
    VERIFY(were_enabled)
    /home/me/dev/pico-os/Kernel/HandlerMode.hpp:0x00000053
    ```

    ```none
    #0  Std::crash (format=0x10015c90 "VERIFY(%condition)\n%file:%line\n", condition=0x10015c80 "were_enabled", file=0x10015c54 "/home/me/dev/pico-os/Kernel/HandlerMode.hpp", line=83) at /home/me/dev/pico-os/Std/Forward.cpp:82
    #1  0x10004d9a in Kernel::MaskedInterruptGuard::MaskedInterruptGuard (this=0x20016fc4) at /home/me/dev/pico-os/Kernel/HandlerMode.hpp:83
    #2  0x1000ba80 in operator() (__closure=0x20016ffc) at /home/me/dev/pico-os/Kernel/SystemHandler.cpp:86
    #3  0x1000c0ac in operator() (__closure=0x20016ff8) at /home/me/dev/pico-os/Kernel/Threads/Thread.hpp:68
    #4  0x1000c46a in type_erased_member_function_wrapper<Kernel::Thread::setup_context<Kernel::SystemHandler::SystemHandler()::<lambda()> >(Kernel::SystemHandler::SystemHandler()::<lambda()>&&)::<lambda()>, &Kernel::Thread::setup_context<Kernel::SystemHandler::SystemHandler()::<lambda()> >(Kernel::SystemHandler::SystemHandler()::<lambda()>&&)::<lambda()>::operator()>(void *) (object=0x20016ff8) at /home/me/dev/pico-os/Std/Forward.hpp:101
    ```

-   In the `SystemHandler`, we were masking interrupts to avoid a lost wakeup problem.
    However, we were also using `MaskedInterruptGuard`.

### Actions

-   Remove the redundant use of `MaskedInterruptGuard`.
