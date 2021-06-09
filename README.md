### TODO

#### Next Version

  - General cleanup

  - Accept UART via interrupt

  - Blocking threads (if required for interrupts)

#### Bugs

  - If we do `stat /dev/tty` we get invalid information, because `ConsoleFileHandle` always
    returns `ConsoleFile` instead of the actual file.

  - We sometimes crash in `Process::create` when computing the new region.
    This appears to be a race condition so the Scheduler is involved?

  - I think, `Singleton<T>`s can be copied and moved?

  - `Optional<T>` should clear source when moving into other object.

#### Future features

  - Run inside QEMU

  - Write userland applications in Zig

#### Future tweaks (Userland)

  - Implement a proper malloc

#### Future tweaks (Kernel)

  - Use RAII to manage `PageRange`s

  - Group `PageRange`s together in `PageAllocator::deallocate`.

  - Provide a shortcut for `Scheduler::the().active_thread()` similar: `Process::current()`.

  - Setup MPU for supervisor mode

#### Future tweaks (Build)

  - Alignment of `.stack`, `.heap` sections is lost in `readelf`

  - Turn on all sorts of warnings

  - C++20 modules

  - Drop SDK entirely

      - Link `libsup++` or add a custom downcast?

  - Meson build

  - Try using LLDB instead of GDB

  - Don't leak includes from newlib libc

  - Build with `-fstack-protector`

  - Use LLVM/LLD for `FileEmbed`; Not sure what I meant with this, but LLVM
    surely has all the tools buildin that I need

  - GDB apparently has a secret 'proc' command that makes it possible to debug
    multiple processes.  This was mentioned in the DragonFlyBSD documentation,
    keyword: "inferiour"

### Helpful Documentation

#### Stuff that I already looked at:

  - ARM ABI (https://github.com/ARM-software/abi-aa/releases)
  - ELF(5) man page (https://man7.org/linux/man-pages/man5/elf.5.html)
  - Ian Lance Taylor: Linkers (https://www.airs.com/blog/archives/38)

### Software

Installed via `pacman`:

~~~none
pacman -S --needed python-invoke arm-none-eabi-gcc
~~~

Some packages have to be manually build from AUR:

- TIO to connect to serial device (http://tio.github.io/)
