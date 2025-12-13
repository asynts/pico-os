# FileSystem Architecture

PicoOS implements a Virtual File System (VFS) abstraction to support different storage backends.

## Architecture Overview

```mermaid
graph TD
    subgraph Userland["User Process"]
        App[Application]
    end

    subgraph Kernel["Kernel VFS Layer"]
        VFS[VirtualFile]
        VFH[VirtualFileHandle]
        VFD[VirtualDirectory]
    end

    subgraph Backends["File System Backends"]
        MFS[MemoryFileSystem<br/>RAM-based]
        DFS[DeviceFileSystem<br/>/dev]
        FFS[FlashFileSystem<br/>Read-only]
    end

    subgraph Hardware["Devices"]
        UART[UART0 Console]
        Flash[Flash Memory]
    end

    App -->|open,read,write| VFH
    VFH --> VFS
    VFS --> MFS
    VFS --> DFS
    VFS --> FFS
    DFS --> UART
    FFS --> Flash
```

---

## VFS Components

### VirtualFile
The base class for all file nodes.

| Property | Type | Description |
| :--- | :--- | :--- |
| `m_mode` | `ModeFlags` | Regular, Directory, or Device |
| `m_size` | `u32` | File size in bytes |
| `m_device_id` | `u32` | Device identifier (if device file) |
| `m_ino` | `u32` | Inode number |

### VirtualFileHandle
Represents an open instance of a file used by a process.
- Tracks read/write offsets
- Must be closed to prevent memory leaks
- Each `open()` creates a new handle

### VirtualDirectory
A special `VirtualFile` containing a map of child entries.
```cpp
HashMap<ImmutableString, VirtualFile*> m_entries;
```

---

## File System Implementations

### 1. MemoryFileSystem (TmpFS)

```mermaid
graph LR
    Root["/"] --> Dev["/dev"]
    Root --> Bin["/bin"]
    Bin --> Shell["Shell.elf"]
    Bin --> Example["Example.elf"]
```

- **Location**: RAM
- **Persistence**: None (lost on reboot)
- **Use Case**: Temporary files, runtime directories

### 2. DeviceFileSystem (`/dev`)

Maps special character devices to file nodes.

| Device | Path | Description |
| :--- | :--- | :--- |
| Console | `/dev/tty` | UART0 input/output |

**Console Implementation**:
- `read()`: Blocks until data available in UART RX FIFO
- `write()`: Sends data to UART TX FIFO

### 3. FlashFileSystem

- **Location**: Flash memory (XIP)
- **Persistence**: Read-only, embedded at build time
- **Generation**: `Tools/ElfEmbed` tool creates embedded ELF sections

**Contents**:
```
/bin/Shell.elf
/bin/Example.elf
/bin/Editor.elf
```

---

## File Operations Flow

```mermaid
sequenceDiagram
    participant App as Application
    participant Kernel as Kernel
    participant VFS as VirtualFile
    participant Handle as VirtualFileHandle

    App->>Kernel: open("/dev/tty", O_RDWR)
    Kernel->>VFS: lookup("/dev/tty")
    VFS-->>Kernel: VirtualFile*
    Kernel->>Handle: create_handle()
    Handle-->>Kernel: VirtualFileHandle&
    Kernel-->>App: fd = 3

    App->>Kernel: write(3, "hello", 5)
    Kernel->>Handle: write(bytes)
    Handle->>VFS: (forward to device)
    VFS-->>Handle: 5 bytes written
    Handle-->>Kernel: 5
    Kernel-->>App: 5

    App->>Kernel: close(3)
    Kernel->>Handle: delete handle
```

---

## Key Files

| File | Description |
| :--- | :--- |
| [VirtualFileSystem.hpp](../../Kernel/FileSystem/VirtualFileSystem.hpp) | Base VFS classes |
| [MemoryFileSystem.cpp](../../Kernel/FileSystem/MemoryFileSystem.cpp) | RAM filesystem |
| [DeviceFileSystem.cpp](../../Kernel/FileSystem/DeviceFileSystem.cpp) | Device node mapping |
| [FlashFileSystem.cpp](../../Kernel/FileSystem/FlashFileSystem.cpp) | Embedded flash files |
| [ConsoleDevice.cpp](../../Kernel/ConsoleDevice.cpp) | TTY implementation |

---

## Known Limitations

- No subdirectory creation at runtime
- No file deletion support
- Flash filesystem is read-only
- Limited error reporting
