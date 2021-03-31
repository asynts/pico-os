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
        VirtualFileHandle& create_file_handle(VirtualFile&) override;
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

        Vector<u8, 0x200> m_data;
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

        KernelResult<usize> read(Bytes bytes) override
        {
            usize nread = m_file->m_data.span().slice(m_offset).copy_trimmed_to(bytes);
            m_offset += nread;

            return nread;
        }
        KernelResult<usize> write(ReadonlyBytes bytes) override
        {
            m_file->m_data.extend(bytes);
            m_offset += bytes.size();
            return bytes.size();
        }

        MemoryFile *m_file;
        usize m_offset = 0;
    };
}
