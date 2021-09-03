### TODO

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

### Development Environment

 1. Install required packages:

    ```none
    pacman -S --needed python-invoke arm-none-eabi-gcc arm-none-eabi-gdb arm-none-eabi-newlib fmt
    ```

 2. Install TIO from AUR:

    ```none
    cdm ~/src/aur.archlinux.org
    git clone --depth 1 https://aur.archlinux.org/tio.git
    cd tio
    makepkg --install
    ```

 3. Build `openocd`:

    ```none
    cdm ~/src/github.com/raspberrypi
    git clone --branch picoprobe --depth 1 git@github.com:raspberrypi/openocd.git
    cd openocd
    ./bootstrap
    CFLAGS=-Wno-error ./configure --enable-picoprobe
    make -j24
    sudo make install
    ```

 4. Build `pico-sdk`:

    ```none
    cdm ~/dev
    git clone --branch tweaks git@github.com:asynts/pico-sdk.git
    ```

 4. Build the project with:

    ```none
    cdm Build
    cmake .. -GNinja -DPICO_SDK_PATH=~/dev/pico-sdk
    ninja
    ```

 4. Connect Raspberry Pi Pico.  The scripts expect two Raspberry devices where
    one is used for debugging and the other runs the operating system. There
    needs to be a UART connection from the debugee to the debugger.

    The debugger runs the picoprobe firmware.

 5. Run `inv probe` to start up `openocd`.

 6. Run `inv tty`, this will be the shell into the target system.

 7. Run `inv dbg`, this will be used for debugging and to load the application.

### Running the System

 1. In the debugger terminal, run `rebuild`.

 2. `run` will start the system.  The shell is accessible in the `inv tty`
    terminal.

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
