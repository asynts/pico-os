### TODO

#### Next Version

-   Add `SoftwareSpinLock` that uses a hardware spin lock but there can be more than actual hardware spin locks.
    This is an active locking primitive that must only be used with interrupts disabled.
    It should be simple to add deadlock detection here.

-   Add `SoftwareMutex` that uses a `HardwareSpinLock` internally.
    This is a passive locking primitive that must only be used with interrupts enabled.

-   Add `WaitingThreadQueue` that keeps a list of threads that are waiting for some resource.
    This must only be used with interrupts disabled and uses a `HardwareSpinLock` internally.

-   Verify that all of these locking primitives are functional.

-   Protect all the resources with these locking primitives.

-   Schedule on both cores.

### Old

#### Next Version

-   Group `PageRange`s together in `PageAllocator::deallocate`.

-   Add passive locking primitives

-   Add active locking primitives

#### Bugs

-   We have a ton of memory leaks in the filesystem, e.g. `VirtualFile::create_handle_impl`.

  - If we do `stat /dev/tty` we get invalid information, because `ConsoleFileHandle` always
    returns `ConsoleFile` instead of the actual file.

  - Sometimes we seem to mess something up, this is visible when `ConsoleFileHandle` has an invalid
    `this` pointer. I reproduced this by running:

    ~~~none
    Example.elf
    Example.elf
    Example.elf
    Example.elf
    Example.elf
    Example.elf
    Example.elf
    ~~~

#### Future features

-   Keep track of 'used' page ranges. There is an excelent algorithm that can be used to store these bits
    in a very compact tree structure.

-   Maybe I could port the Minix filesystem when I add an IDE driver?

-   Keep documentation about interrupt safe functions and which functions can be called in which boot stage

-   Add `MemoryAllocator::allocate_eternal` which doesn't create MTRACE logs

  - Run inside QEMU

  - Write userland applications in Zig

#### Future tweaks (Userland)

  - Implement a proper malloc

#### Future tweaks (Kernel)

  - Setup MPU for supervisor mode

  - HardFault in usermode crashes kernel

  - Stack smash protection with MPU

      - Build with `-fstack-protector`?

#### Future tweaks (Build)

  - Alignment of `.stack`, `.heap` sections is lost in `readelf`

  - C++20 modules

  - Drop SDK entirely

      - Link `libsup++` or add a custom downcast?

  - Meson build

  - Try using LLDB instead of GDB

  - Don't leak includes from newlib libc

  - Use LLVM/LLD for `FileEmbed`; Not sure what I meant with this, but LLVM
    surely has all the tools buildin that I need

  - GDB apparently has a secret 'proc' command that makes it possible to debug
    multiple processes.  This was mentioned in the DragonFlyBSD documentation,
    keyword: "inferiour"
