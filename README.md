<div align="center">

# PicoOS

**PicoOS** is a simple, microkernel-inspired operating system designed for the Raspberry Pi Pico (RP2040) microcontroller. It features a custom kernel, a virtual file system, multi-core locking primitives, and a basic shell.

![Screen Capture](Docs/images/demo.gif)

</div>

## 📖 Documentation

Full documentation is available in the [Docs/](Docs/index.md) directory.

*   [**Getting Started**](Docs/getting_started/index.md): Installation, build instructions, and flashing.
*   [**Architecture**](Docs/architecture/system_overview.md): System design, kernel internals, and synchronization.
*   [**Reference**](Docs/reference/syscalls.md): System calls and API usage.

## ✨ Features

*   **Platform**: Runs natively on RP2040 (Raspberry Pi Pico).
*   **Kernel**:
    *   Round-robin Scheduler.
    *   Wait-state efficient Synchronization (`SoftwareSpinLock`, `SoftwareMutex`).
    *   Virtual File System (VFS) with `/dev` and Flash support.
    *   MPU-based memory protection.
*   **Userland**:
    *   ELF Executable loading (`posix_spawn`).
    *   Basic Shell and Editor.
    *   Standard C Library (partial implementation).

## 🚀 Quick Start

1.  **Build**:
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

    5. Build the project with:

    ```none
    cdm Build
    cmake .. -GNinja -DPICO_SDK_PATH=~/dev/pico-sdk
    ninja
    ```

    6. Connect Raspberry Pi Pico.  The scripts expect two Raspberry devices where
    one is used for debugging and the other runs the operating system. There
    needs to be a UART connection from the debugee to the debugger.
    The debugger runs the picoprobe firmware.

2.  **Flash**:
    Connect Pico in BOOTSEL mode and run:
    ```bash
    inv flash
    ```

3.  **Connect**:
    ```bash
    inv tty
    ```

See [Getting Started](Docs/getting_started/index.md) for full details.
