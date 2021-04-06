#pragma once

#include <Std/Singleton.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>
#include <Kernel/Interface/stat.h>

namespace Kernel
{
    using namespace Std;

    class MemoryFileSystem;
    class MemoryFile;
    class MemoryDirectory;
    class MemoryFileHandle;

    class MemoryFileSystem final
        : public Singleton<MemoryFileSystem>
        , public VirtualFileSystem
    {
    public:
        VirtualFile& root() override;

        VirtualFile& create_file() override;

        u32 next_ino() { return m_next_ino++; }

    private:
        friend Singleton<MemoryFileSystem>;
        MemoryFileSystem();

        MemoryDirectory *m_root;
        u32 m_next_ino = 2;
    };

    class MemoryFile final : public VirtualFile {
    public:
        MemoryFile()
        {
            m_ino = MemoryFileSystem::the().next_ino();
            m_size = 0;
        }

        VirtualFileSystem& filesystem() override { return MemoryFileSystem::the(); }

        ReadonlyBytes span() const { return m_data.span(); }
        Bytes span() { return m_data.span(); }

        VirtualFileHandle& create_handle() override;

        void append(ReadonlyBytes bytes)
        {
            m_data.extend(bytes);
        }

    private:
        Vector<u8> m_data;
    };

    class MemoryFileHandle final : public VirtualFileHandle {
    public:
        VirtualFile& file() override { return *m_file; }

        KernelResult<usize> read(Bytes bytes) override
        {
            usize nread = m_file->span().slice(m_offset).copy_trimmed_to(bytes);
            m_offset += nread;

            return nread;
        }
        KernelResult<usize> write(ReadonlyBytes bytes) override
        {
            m_file->append(bytes);
            m_offset += bytes.size();
            return bytes.size();
        }

        MemoryFile *m_file;
        usize m_offset = 0;
    };

    // FIXME: Hook this up
    class MemoryDirectory final : public VirtualFile {
    public:
        MemoryDirectory()
        {
            m_ino = MemoryFileSystem::the().next_ino();
            m_size = 0;
            m_mode = ModeFlags::Directory;

            m_entries.set(".", this);
            m_entries.set("..", this);
        }

        VirtualFileSystem& filesystem() override { return MemoryFileSystem::the(); }

        VirtualFileHandle& create_handle() override;

        HashMap<String, VirtualFile*> m_entries;
    };

    class MemoryDirectoryHandle final : public VirtualFileHandle {
    public:
        VirtualFile& file() override { return *m_file; }

        KernelResult<usize> read(Bytes bytes) override
        {
            UserlandDirectoryInfo info;

            // FIXME: Implement iterators for Std::Map, maybe do it properly now?
            auto iter = m_file->m_entries.iter();
            for (usize index = 0; index < m_offset; ++index)
                ++iter;
            (*iter).m_key.strcpy_to({ info.d_name, sizeof(info.d_name) });

            ++m_offset;
            return bytes_from(info).copy_to(bytes);
        }
        KernelResult<usize> write(ReadonlyBytes bytes) override
        {
            VERIFY_NOT_REACHED();
        }

        MemoryDirectory *m_file;
        usize m_offset = 0;
    };
}
