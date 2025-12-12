# PicoOS Documentation

Welcome to the documentation for **PicoOS**, a simple operating system designed for the Raspberry Pi Pico (RP2040) microcontroller.

---

## 🚀 Getting Started

| Document | Description |
| :--- | :--- |
| [Installation & Setup](getting_started/index.md) | Prerequisites, build instructions, and flashing |
| [Debugging](getting_started/debugging.md) | Using OpenOCD, GDB, and VSCode |

---

## 🏗️ Architecture

| Document | Description |
| :--- | :--- |
| [System Overview](architecture/system_overview.md) | Memory model, kernel/userland split, MPU |
| [Scheduler](architecture/scheduler.md) | Preemptive round-robin, context switching |
| [Synchronization](architecture/synchronization.md) | Multi-core locks: `SoftwareSpinLock`, `SoftwareMutex` |
| [FileSystem](architecture/filesystem.md) | VFS, `/dev`, Flash memory |

---

## 📚 Reference

| Document | Description |
| :--- | :--- |
| [System Calls](reference/syscalls.md) | Complete syscall reference with examples |
| [Shell Commands](reference/shell.md) | Built-in shell commands |

---

## 🛠️ Development

| Document | Description |
| :--- | :--- |
| [Workflow](development/workflow.md) | Adding syscalls, running tests, code style |
| [Debug Notes](development/debug_notes/) | Historical debugging logs |

---

## 📊 Quick Reference

### Build Commands
```bash
mkdir build && cd build
cmake .. -G Ninja
ninja
```

### Flash Firmware
```bash
inv flash  # Pico must be in BOOTSEL mode
```

### Connect to Shell
```bash
inv tty
```

### Start Debugging
```bash
inv probe  # Terminal 1: OpenOCD
inv dbg    # Terminal 2: GDB
```
