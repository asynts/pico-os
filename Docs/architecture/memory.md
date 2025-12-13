# Memory Management & Security

PicoOS utilizes a robust set of memory management and security features tailored for the RP2040 (Cortex-M0+). Despite lacking an MMU (Memory Management Unit), it implements strong isolation and safety mechanisms.

## Memory Allocators

PicoOS uses a two-level memory allocation architecture:

### 1. Physical Page Allocator (Buddy System)
*   **File**: `Kernel/PageAllocator.cpp`
*   **Algorithm**: Binary Buddy System
*   **Unit**: 4KB Pages
*   **Total Managed Memory**: ~260KB (excluding bootloader and stack scratch areas)
*   **Use Case**: Allocating large, contiguous physical memory blocks for the Kernel Heap, Process Stacks, or File Buffers.

The Page Allocator manages the raw SRAM. It tracks usage using a `SortedSet` of allocated ranges, allowing for detailed debugging and leak detection.

### 2. Global Memory Allocator (Kernel Heap)
*   **File**: `Kernel/GlobalMemoryAllocator.cpp`
*   **Algorithm**: Free-List (Variable size)
*   **Features**:
    *   **Dynamic Growth**: Starts with a small initial heap (16KB) and requests additional pages from the Page Allocator as needed.
    *   **Thread Safety**: Protected by `malloc_mutex` (Software Mutex) with `ScopedLock`.
    *   **Standard Compliance**: Implements `malloc`, `free`, `calloc`, `realloc`, and `reallocarray` (with overflow protection).
    *   **Eternal Allocation**: `allocate_eternal()` for persistent kernel structures that should not be tracked by leak detectors.

---

## Security Features

### Memory Protection Unit (MPU)
Since the Cortex-M0+ lacks virtual memory, PicoOS uses the MPU to enforce privilege levels:
*   **Supervisor Mode (Kernel)**: Full access to all memory and peripherals.
*   **User Mode (Userland)**:
    *   **Code**: Execute-only (XOM) from RAM or Flash.
    *   **Data**: Read/Write access only to its own stack and heap.
    *   **No Access**: Blocked from accessing Kernel RAM, Peripherals (except specific allowed zones), or other processes.

### Stack Smashing Protection (SSP)
PicoOS implements GCC's `-fstack-protector-strong` to detect buffer overflows.
*   **Hardware Randomization**: The stack canary (`__stack_chk_guard`) is seeded at boot using the RP2040's hardware Ring Oscillator (ROSC) random bit generator (`init_stack_guard()`).
*   **Detection**: If a function return address is overwritten, the corrupted canary triggers `__stack_chk_fail`, causing a Kernel Panic.

### Exception Handling
*   **HardFault Handler**: Differentiates between User and Kernel faults.
    *   **User Fault**: Kills the offending thread/process but keeps the OS running.
    *   **Kernel Fault**: Panics the system with a register dump and stack trace for debugging.

---

## Memory Map (RP2040)

| Region | Address | Size | Description |
| :--- | :--- | :--- | :--- |
| **Bootrom** | `0x00000000` | 16 KB | Internal Read-Only Bootloader |
| **Flash (XIP)**| `0x10000000` | 2 MB | Operating System Code & Read-Only Data |
| **SRAM** | `0x20000000` | 264 KB | Main System Memory |
| **Peripherals**| `0x40000000` | | Memory Mapped IO (UART, DMA, FIFO, etc.) |
| **SIO** | `0xD0000000` | | Single Cycle IO (Spinlocks, CPUID, GPIO) |
| **PPB** | `0xE0000000` | | ARM Core Peripherals (NVIC, SysTick, MPU) |

### SRAM Layout

| Start | usage |
| :--- | :--- |
| `0x20000000` | **Kernel Data** (.data, .bss) |
| `0x20002xxx` | **Kernel Heap** (Dynamically grows upwards) |
| ... | **Free Memory** (Managed by PageAllocator) |
| `0x20040000` | **Scratch X** (Core 1 Stack / Buffers) |
| `0x20041000` | **Scratch Y** (Core 0 Stack / Buffers) |
