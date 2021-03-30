#pragma once

#include <Std/Map.hpp>
#include <Std/String.hpp>

namespace Kernel
{
    using namespace Std;

    class VirtualFile;
    class VirtualFileHandle;
    class VirtualDirectoryEntry;
    class VirtualFileSystem;

    class VirtualFileSystem {
    public:
        virtual VirtualFile& create_file() = 0;
        virtual VirtualFileHandle& create_file_handle() = 0;
        virtual VirtualDirectoryEntry& create_directory_entry() = 0;
    };

    class VirtualFile {
    public:
        u32 m_ino;
        u32 m_flags;
        u32 m_size;
        u32 m_device;

        virtual VirtualFileSystem& filesystem() = 0;
    };

    class VirtualDirectoryEntry {
    public:
        virtual VirtualFile& file() = 0;

        Map<String, VirtualDirectoryEntry*> m_entries;
    };

    class VirtualFileHandle {
    public:
        virtual VirtualFile& file() = 0;
    };
}
