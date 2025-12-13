# Debugging PicoOS

Effective debugging is essential for OS development. PicoOS supports hardware debugging via **SWD** (Serial Wire Debug) using a **PicoProbe** or similar adapter.

## Hardware Setup

**Required**:
1.  **Target Pico**: The one running PicoOS.
2.  **Debug Probe**: Another Pico running [picoprobe](https://github.com/raspberrypi/picoprobe) firmware, or a J-Link.
3.  **Wiring**:
    *   Probe GND -> Target GND
    *   Probe GP2 (SWCLK) -> Target SWCLK
    *   Probe GP3 (SWDIO) -> Target SWDIO
    *   Probe GP4 (UART TX) -> Target GP1 (UART RX)
    *   Probe GP5 (UART RX) -> Target GP0 (UART TX)

## Using OpenOCD

We use open-source OpenOCD to interface with the probe.

1.  **Start OpenOCD**:
    ```bash
    inv probe
    ```
    *This runs: `openocd -f interface/picoprobe.cfg -f target/rp2040.cfg`*

## Using GDB

Once OpenOCD is running (port 3333):

1.  **Start GDB**:
    ```bash
    inv dbg
    ```
    *This loads `build/Kernel.elf` and connects to localhost:3333.*

2.  **GDB Helper Commands**:
    The `inv dbg` task sets up helpful macros:
    *   `rebuild`: Runs ninja, reloads symbols, resets target.
    *   `si_and_dis`: Step instruction and disassemble.

## VSCode Integration

The project includes VSCode configuration files for a seamless experience.

### Setup
1.  Install the **Cortex-Debug** extension.
2.  Install the **C/C++** extension.
3.  Ensure `arm-none-eabi-gdb` is in your PATH.

### Launch Configurations
*   **Cortex Debug: Attach**: Connects to a running OpenOCD session or starts one, attaches to the Kernel, and pauses execution.
    *   Select this from the logical "Run and Debug" tab.
    *   Press **F5**.

It maps source lines to assembly steps, allows variable inspection, and breakpoint management.
