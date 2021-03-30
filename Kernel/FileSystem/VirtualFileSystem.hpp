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

    enum class ModeFlags {
        DIRECTORY,
        DEVICE,
    };

    class VirtualFileSystem {
    public:
        virtual VirtualDirectoryEntry& root() = 0;

        virtual VirtualFile& create_file() = 0;
        virtual VirtualFileHandle& create_file_handle() = 0;
        virtual VirtualDirectoryEntry& create_directory_entry() = 0;
    };

    class VirtualFile {
    public:
        u32 m_ino;
        ModeFlags m_mode;
        u32 m_size;
        u32 m_device;

        virtual VirtualFileSystem& filesystem() = 0;

        VirtualFileHandle& create_file_handle()
        {
            auto& handle = filesystem().create_file_handle();
            handle. // FUCK
        }
    };

    class VirtualDirectoryEntry {
    public:
        virtual VirtualFile& file() = 0;

        void ensure_loaded()
        {
            if (!m_loaded)
                load();

            m_loaded = true;
        }

        Map<String, VirtualDirectoryEntry*> m_entries;
        bool m_loaded = false;

    protected:
        virtual void load() = 0;
    };

    class VirtualFileHandle {
    public:
        virtual VirtualFile& file() = 0;
    };
}
