# Getting Started with PicoOS

## Prerequisites

To build and develop PicoOS, you need the following tools:

### Cross-Compiler
*   **ARM GNU Toolchain**: `arm-none-eabi-gcc`, `arm-none-eabi-g++`, `arm-none-eabi-gdb`.
    *   *macOS*: `brew install --cask arm-gnu-toolchain` (or download from ARM website).
    *   *Linux*: `sudo apt install gcc-arm-none-eabi gdb-multiarch` (or package manager equivalent).

### Build Tools
*   **CMake** (3.19+)
*   **Ninja** (Recommended generator)
*   **Python 3** (for `invoke` tasks)
*   **Git**

### Dependencies
*   **Pico SDK**: The project automatically fetches the SDK if not found, but setting `PICO_SDK_PATH` is recommended.
*   **Python Packages**: `invoke` (`pip install invoke`).
*   **fmt**: C++ formatting library (often installed via system package manager or CMake will look for it).

---

## Setting Up

1.  **Clone the Repository**:
    ```bash
    git clone https://github.com/asynts/pico-os.git
    cd pico-os
    ```

2.  **Install Python Dependencies**:
    ```bash
    pip3 install invoke
    ```

---

## <a name="building"></a>Building the Project

We use `cmake` and `ninja`.

1.  **Create a Build Directory**:
    ```bash
    mkdir build
    cd build
    ```

2.  **Configure CMake**:
    ```bash
    # You might need to specify PICO_SDK_PATH if not in environment
    cmake .. -G Ninja
    ```

3.  **Build**:
    ```bash
    ninja
    ```

    This will generate:
    *   `build/Kernel.elf`: ELF file for debugging (includes symbols).
    *   `build/Kernel.1.uf2`: UF2 firmware image for flashing.

---

## Flashing

### Using UF2 (USB Mass Storage)
1.  Connect the Raspberry Pi Pico to your computer while holding the **BOOTSEL** button.
2.  The Pico will appear as a USB Mass Storage device named `RPI-RP2`.
3.  Run the helper task:
    ```bash
    inv flash
    ```
    *Or manually copy the file:*
    ```bash
    cp build/Kernel.1.uf2 /Volumes/RPI-RP2/
    ```

### Using PicoProbe (Debugging)
If you have a second Pico set up as a probe:
1.  Connect the Probe to the target Pico (SWD and UART).
2.  Flash using GDB (see [Debugging](debugging.md)).

---

## Interacting with the Shell

PicoOS exposes a shell via the UART0 interface (GP0/TX, GP1/RX).

1.  **Determine your Serial Port**:
    *   *macOS*: `/dev/tty.usbmodem*` or `/dev/cu.usbmodem*`
    *   *Linux*: `/dev/ttyUSB*` or `/dev/ttyACM*`

2.  **Connect**:
    You can use the helper task:
    ```bash
    inv tty
    ```
    Or use `screen` / `tio` directly:
    ```bash
    screen /dev/tty.usbmodem123456 115200
    ```
