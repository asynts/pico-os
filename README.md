<div align="center">

# PicoOS

**PicoOS** is a simple, microkernel-inspired operating system designed for the Raspberry Pi Pico (RP2040) microcontroller. It features a custom kernel, a virtual file system, multi-core locking primitives, and a basic shell.

![Screen Capture](Docs/images/demo.gif)

</div>

## Documentation

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
    *   MPU-based memory protection (Supervisor/User isolation).
    *   **Security**: Stack Smashing Protection (SSP) with Hardware RNG (ROSC).
    *   **Memory**: Dynamic Kernel Heap, Buddy System Page Allocator.
*   **Userland**:
    *   ELF Executable loading (`posix_spawn`).
    *   Basic Shell and Editor.
    *   Standard C Library (partial implementation).

## 🚀 Quick Start

### 1. Prerequisites

You will need the following tools:
*   **Toolchain**: `arm-none-eabi-gcc`, `newlib`
*   **Build System**: `cmake`, `ninja`
*   **Debug**: `gdb-multiarch` (or `arm-none-eabi-gdb`), `openocd`
*   **Utilities**: `python3`, `tio` (serial terminal)

#### Ubuntu / Debian
```bash
sudo apt update
sudo apt install build-essential cmake ninja-build python3-invoke \
    gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib \
    gdb-multiarch libfmt-dev \
    automake autoconf texinfo libtool libftdi-dev libusb-1.0-0-dev # For OpenOCD
```

#### Arch Linux
```bash
sudo pacman -S --needed python-invoke arm-none-eabi-gcc arm-none-eabi-gdb \
    arm-none-eabi-newlib fmt ninja cmake openocd
```
*Note: For `tio`, install from AUR (e.g., `yay -S tio`).*

### 2. Build OpenOCD (Ubuntu/Debian)
Arch Linux users can skip this if they installed the `openocd` package.

```bash
git clone https://github.com/raspberrypi/openocd.git --branch rpi-common --depth=1
cd openocd
git submodule update --init --recursive
./bootstrap
./configure --enable-picoprobe --enable-internal-jimtcl
make -j$(nproc)
sudo make install
```

### 3. Build PicoOS
```bash
mkdir build && cd build
# PICO_SDK_FETCH_FROM_GIT=ON automatically downloads the SDK
cmake .. -G Ninja -DPICO_SDK_FETCH_FROM_GIT=ON
ninja
```
This generates `build/Kernel.1.uf2`.

### 4. Flash and Connect
1.  **Flash**: Hold BOOTSEL on Pico, connect USB, and run:
    ```bash
    inv flash
    ```
2.  **Connect**:
    ```bash
    inv tty
    ```

See [Getting Started](Docs/getting_started/index.md) for full details.
