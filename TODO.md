# TODO

## ✅ Completed (2025-12-14)

- [x] Refactor `Tools/` directory (ElfEmbed, FileSystem) to use modern C++20 and `std::filesystem`.
- [x] Verify and fix build process on Ubuntu/Debian.
- [x] Implement dynamic kernel heap growth.
- [x] Streamline `README.md` installation instructions.
- [x] Update `CHANGELOG.md` with changes since last release.

## ✅ Completed (2025-12-12)

- [x] macOS build compatibility (ELF definitions, portable file I/O)
- [x] Fix missing `MaskedInterruptGuard` includes across kernel
- [x] Fix stack alignment issues causing potential hardfaults
- [x] Implement `init_array`/`fini_array` calling in crt0 (C++ static constructor support)
- [x] Fix linker warnings (executable stack, missing symbols)
- [x] All unit tests passing (18/18)
- [x] Added `AbstractLock` and `LockGuard`
- [x] Added `HardwareSpinLock` using memory-mapped spin locks

### Synchronization Primitives

- [x] Add `SoftwareSpinLock` that uses a hardware spin lock but allows more than actual hardware spin locks.
  - Active locking primitive, must only be used with interrupts disabled.
  - [x] Should include deadlock detection.
  - [x] Includes `wfe`/`sev` for power efficiency.

- [x] Add `SoftwareMutex` that uses a `HardwareSpinLock` internally.
  - Passive locking primitive, must only be used with interrupts enabled.

- [x] Add `WaitingThreadQueue` that keeps a list of threads waiting for a resource.
  - Must only be used with interrupts disabled, uses `HardwareSpinLock` internally.

- [x] Verify all locking primitives are functional.

- [x] Protect all shared resources with these locking primitives.

### Multi-Core Support

- [x] Schedule on both cores.
- [x] Fix `stat /dev/tty`.
- [x] Implement proper malloc (free-list).
- [x] Upgrade sorted set to RB Tree.
- [x] Track allocated pages in `PageAllocator`.

---

## 🐛 Known Bugs

- [x] Memory leaks in filesystem (e.g., `VirtualFile::create_handle_impl`).

- [x] `stat /dev/tty` returns invalid information because `ConsoleFileHandle` always returns `ConsoleFile` instead of actual file.

- [x] Intermittent `ConsoleFileHandle` invalid `this` pointer when running:
  ```
  Example.elf
  Example.elf
  Example.elf
  ... (repeated)
  ```

---

## 📋 Future Features

### Kernel:
- [x] Keep track of 'used' page ranges with compact tree structure
- [ ] Port Minix filesystem when IDE driver is added
- [x] Document interrupt-safe functions and boot stage compatibility
- [x] Add `MemoryAllocator::allocate_eternal` without MTRACE logs
- [ ] Run inside Emulation (Blocked: Mainline QEMU/Renode issues, see [Docs/Emulation_Status.md](Docs/Emulation_Status.md))
- [x] Setup MPU for supervisor mode
- [x] HardFault in usermode should not crash kernel
- [x] Stack smash protection with MPU (`-fstack-protector`) and ROSC randomization
- [x] Group `PageRange`s together in using Buddy coalescing
- [x] Fix alignment of `.stack`, `.heap` sections in `linker.ld`

### Userland:
- [x] Implement a proper malloc (current is bump allocator with no free)
- [ ] Write userland applications in Zig

### Build System
- [x] Group `PageRange`s together in `PageAllocator::deallocate` (Buddy optimization implemented)
- [x] Fix alignment of `.stack`, `.heap` sections in `readelf` (Verified 8-byte alignment)
- [ ] C++20 modules support
- [ ] Drop SDK entirely (link `libsup++` or add custom downcast)
- [ ] Meson build support
- [ ] Try LLDB instead of GDB
- [ ] Don't leak includes from newlib libc
- [x] Use LLVM/LLD for `FileEmbed` (Evaluated: Custom Tool `Tools/ElfEmbed.cpp` needed for FileSystem structure)
- [ ] Explore GDB 'proc' command for multi-process debugging ("inferior") FileSystem structure
