# PicoOS

This is a simple operating system for the Raspberry Pi Pico micro-controller.
The code structure is strongly inspired by SerenityOS, however, no code is taken from that source directly.

My goal was to write a simple operating system because I was interested in operating systems.
The code base is a huge mess and I later tried to rewrite everything, however, I never completed the rewrite.

Currently the system has the following capabilities:

-   The system runs on a Raspberry Pi Pico microcontroller (actual hardware!).
    It uses a UART connection to communicate with a terminal window of the host machine.

    Instead of connecting the device directly, it is connected indirectly with another Raspberry Pi Pico which has the PicoProbe
    software instealled.
    This makes it possible to debug what is happening on the chip and the UART connection is exposed via USB.

-   There is a very fragile file system implementation that supports most common operations.
    Some programs are embedded into the flash memory of the device and they are accessible in this file system.

-   The system itself has an extremely simple shell program which is loaded on startup and which is accessible with the UART connection.

    There are some builtin shell commands, but it can also use `posix_spawn` to start a new process.
    This can be used to load any ELF file, but the system makes a ton of assumptions about the application.

The kernel has the following capabilities:

-   There is a bare bone memory allocation algorithm.

-   There is a scheduler that must run on a single core.
    It can switch between several threads that belong either to the kernel or to userland.

    I tried adding multi-core support later on, however, the debugging tools I was using were not sophisticated enough to debug what went wrong.

-   There is some bare bone isolation between kernel and userland.

    Sadly, this microcontroller does not have a Memory Management Unit (MMU), therefore, proper isolation isn't possible.
    However, the Memory Protection Unit (MPU) is used to at least prevent accesses to kernel space.

-   The following system calls are partially supported: `read`, `write`, `open`, `close`, `fstat`, `wait`, `exit`,
    `chdir`, `get_working_directory`, `posix_spawn`.

    Notice, that `fork` is not on that list since it requires an MMU, however, `posix_spawn` can do most of the things that `fork` can do.

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
