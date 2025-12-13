# Development Workflow

## Adding a New System Call

1.  **Define ID**: Add a new `_SC_name` constant in `Kernel/Interface/System.hpp`.
2.  **Kernel Implementation**:
    *   Add declaration to `Thread` class in `Kernel/Threads/Thread.hpp`.
    *   Implement `sys$name` in `Kernel/Threads/Thread.cpp`.
    *   Add case to `Thread::syscall` dispatch switch.
3.  **Userland Wrapper**:
    *   Implement function in `Userland/LibC/sys/syscalls.cpp` using `syscall0`...`syscall3` macros.
    *   Expose header in `Userland/LibC` (or standard headers).

## Running Tests

### Standard Library Tests
The core data structures are tested on the host machine.
1.  Navigate to `Tests/` or use the main build.
2.  (See `Tests/CMakeLists.txt` for details).

### Kernel Verification
Currently requires `QEMU` or actual hardware.
1.  Build `Kernel.1.uf2`.
2.  Flash to Pico.
3.  Observe output via UART (`inv tty`).

## Code Style
*   Use `PascalCase` for classes/structs.
*   Use `snake_case` for functions and variables.
*   Use `m_` prefix for member variables.
*   Use `AK` (SerenityOS) style references (`RefPtr`, `OwnPtr`) where possible.
