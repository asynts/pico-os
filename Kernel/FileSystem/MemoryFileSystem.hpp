#pragma once

#include <Std/Singleton.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel
{
    using namespace Std;

    class MemoryFileSystem;
    class MemoryFile;
    class MemoryFileHandle;
    class MemoryDirectoryEntry;

    class MemoryFileSystem final
        : public Singleton<MemoryFileSystem>
        , public VirtualFileSystem
    {
    public:
        VirtualDirectoryEntry& root() override { return *m_root; }

        VirtualFile& create_file() override;
        VirtualFileHandle& create_file_handle() override;
        VirtualDirectoryEntry& create_directory_entry() override;

    private:
        friend Singleton<MemoryFileSystem>;
        MemoryFileSystem();

        MemoryDirectoryEntry *m_root;
    };

    class MemoryFile final
        : public VirtualFile
    {
    public:
        VirtualFileSystem& filesystem() override { return MemoryFileSystem::the(); }
    };

    class MemoryDirectoryEntry final
        : public VirtualDirectoryEntry
    {
    public:
        VirtualFile& file() override { return *m_file; }

        void load() override { /* nop */ }

        MemoryFile *m_file;
    };

    class MemoryFileHandle final
        : public VirtualFileHandle
    {
    public:
        VirtualFile& file() override { return *m_file; }

        MemoryFile *m_file;
    };
}
