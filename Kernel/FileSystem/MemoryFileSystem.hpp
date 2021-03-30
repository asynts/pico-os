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
        VirtualDirectoryEntry& root() override;

        VirtualFile& create_file() override;
        VirtualFileHandle& create_file_handle() override;
        VirtualDirectoryEntry& create_directory_entry() override;

        u32 next_ino() { return m_next_ino++; }

    private:
        friend Singleton<MemoryFileSystem>;
        MemoryFileSystem();

        MemoryDirectoryEntry *m_root;
        u32 m_next_ino;
    };

    class MemoryFile final
        : public VirtualFile
    {
    public:
        MemoryFile()
        {
            m_ino = MemoryFileSystem::the().next_ino();
        }

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
