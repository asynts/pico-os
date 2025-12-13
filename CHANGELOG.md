# Changelog

## [Unreleased] - 2025-12-14

### Added
- **Final Polish Items**:
    - `SoftwareSpinLock`: Active locking with deadlock detection and power efficiency (`wfe`/`sev`).
    - `SoftwareMutex`: Passive locking using `HardwareSpinLock`.
    - `WaitingThreadQueue`: For managing threads waiting on resources.
- **Hardware Integration**:
    - `HardwareSpinLock`: Memory-mapped spin lock support for RP2040.
    - `sys$read`, `sys$write`: Basic system calls hooked to `ConsoleDevice`.
    - **MPU**: Supervisor/User memory separation verified.
- **Core Infrastructure**:
    - `init_array`/`fini_array` support in crt0 for C++ static constructors.
    - Stack alignment fixes to prevent hardfaults.
    - **Exception Handling**: Implemented HardFault handler (Userland kill, Kernel panic).
    - **Memory**: Added `allocate_eternal` for silent allocation.
    - **QEMU**: Added launch script `Tools/run-qemu.sh`.

### Refactoring & Core Improvements
- **Kernel/StackProtector**: Implemented `init_stack_guard` using RP2040 ROSC for hardware-randomized stack canaries.
- **Kernel/Exceptions**: Enhanced HardFault handlers with clearer register dumps and stack trace headers.
- **Kernel/PageAllocator**: Added detailed comments on RP2040 RAM layout and Buddy System initialization.
- **Kernel/GlobalMemoryAllocator**: Standardized mutex usage with `ScopedMutex` and added comments. Implemented dynamic heap growth.
- **Kernel/StackWrapper**: Improved API docs and parameter naming.
- **Tools**: Removed unstable QEMU/Renode scripts (emulation deferred).

### System
- **Synchronization**: `HardwareSpinLock` is now ISR-safe, storing/restoring interrupt state.
- **MPU**: Setup for supervisor/user mode isolation.
- **Build**: Linker script (`linker.ld`) enforces 8-byte alignment for stack/heap.
- **Documentation**:
    - Added `docs/InterruptSafety.md`.
- **Multi-Core Support**: `Scheduler` now supports creating and scheduling threads on both cores of the RP2040. Integrated `HardwareSpinLock` for safe inter-core synchronization.
- **Robust Data Structures**:
    - Replaced `Std::SortedSet`'s binary search tree with a **Red-Black Tree** implementation, guaranteeing O(log n) operations.
    - Added `m_allocated_pages` to `PageAllocator` using `SortedSet` to explicitly track allocated memory ranges.
- **Userland Allocator**: Implemented a free-list based `malloc`/`free`/`realloc`/`calloc` in `Userland/LibC`, replacing the previous bump allocator. This allows memory reuse and reduces fragmentation.
- `Std::Formatter` specialization for `Kernel::PageRange`.

### Changed
- Refactored `ConsoleFileHandle` to store a reference to the specific `VirtualFile` instance, fixing an issue where `stat /dev/tty` returned incorrect device information.
- Updated `Scheduler::dump()` to correctly iterate over core-specific thread arrays.
- Enhanced `PageAllocator::dump()` to show detailed allocation tracking.

### Fixed
- `stat /dev/tty` listing incorrect file attributes by decoupling `ConsoleFileHandle` from the singleton.
- Compilation errors in `Std/SortedSet.hpp` related to `HashTable` compatibility (relied on strict weak ordering `<` instead of `==`).

## [Unreleased] - 2025-12-12

### Fixed
- **macOS Build Compatibility**: Added `Tools/compat/elf.h` with minimal ELF definitions for macOS.
- **Portable File I/O**: Replaced Linux-specific `memfd_create`/`copy_file_range` with ANSI C alternatives (`tmpfile`, `fread`/`fwrite`).
- **Missing Includes**: Added `MaskedInterruptGuard.hpp` includes to `Process.cpp`, `SystemHandler.cpp`, `Loader.cpp`, `Scheduler.cpp`, and `KernelMutex.hpp`.
- **Stack Alignment**: Fixed potential hardfaults by ensuring 8-byte stack alignment in `Thread::setup_context()` and `hand_over_to_loaded_executable()`.
- **strlcpy Conflict**: Updated `Std/Forward.hpp` to use standard `size_t` return type.
- **Linker Symbol Resolution**: Hardcoded RP2040 RAM boundaries in `PageAllocator.cpp` to bypass missing linker script definitions.
- **Iterator Const Correctness**: Fixed `SortedSet::InorderIterator` comparison operators to accept const references.
- **Test Suite**: Fixed narrowing conversion, undeclared identifiers, and string arithmetic warnings in test files.

### Added
- **C++ Static Constructors**: Implemented `preinit_array`, `init_array`, and `fini_array` calling in `Userland/LibC/sys/crt0.c`.
- **Non-Executable Stack**: Added `.note.GNU-stack` section to `crt0.S` to silence linker warnings.

### Changed
- Refactored `HardwareSpinLock` to correct lock acquisition logic (removed infinite loop on success) and added `wfe`/`sev` instructions for power-efficient SMP synchronization.
- Enhanced `SoftwareSpinLock` with **Deadlock Detection** (checks if core already holds the lock) and `wfe`/`sev` support.
- Updated `.gitignore` to allow useful VSCode configuration files.
- Removed `-lbsd` dependency from `Tools/CMakeLists.txt`.
- Removed unsupported `-fropi`/`-frwpi` compiler flags from `Userland/CMakeLists.txt`.
- Commented out custom `operator new`/`delete` in `GlobalMemoryAllocator.cpp` to avoid conflicts with Pico SDK.

### Added
- **VSCode Configuration**: Added `c_cpp_properties.json` for IntelliSense and `launch.json` for Cortex-Debug support.
- **C++ Static Constructors**: Implemented `preinit_array`, `init_array`, and `fini_array` calling in `Userland/LibC/sys/crt0.c`.

---

## Previous Changes

### Major

-   Added `AbstractLock` which is used by the new `LockGuard`.

-   Added `HardwareSpinLock` which uses the memory mapped spin locks that the processor provides.
