#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>
#include <Kernel/Interface/Types.hpp>

namespace Kernel
{
    using namespace Std;

    class FlashFileSystem;
    class FlashFile;
    class FlashDirectory;
    class FlashFileHandle;

    class FlashFileSystem final
        : public Singleton<FlashFileSystem>
        , public VirtualFileSystem
    {
    public:
        VirtualFile& root() override;

    private:
        friend Singleton<FlashFileSystem>;
        FlashFileSystem();

        FlashDirectory *m_root;
    };

    class FlashFile final : public VirtualFile {
    public:
        explicit FlashFile(FileInfo& info)
            : m_data(info.m_data, info.st_size)
        {
            m_ino = info.st_ino;
            m_mode = info.st_mode;
        }

        VirtualFileHandle& create_handle() override;

        ReadonlyBytes m_data;
    };

    class FlashFileHandle final : public VirtualFileHandle {
    public:
        explicit FlashFileHandle(FlashFile& file)
            : m_file(file)
            , m_offset(0)
        {
        }

        KernelResult<usize> read(Bytes bytes) override
        {
            usize nread = m_file.m_data.slice(m_offset).copy_trimmed_to(bytes);
            m_offset += nread;
            return nread;
        }

        KernelResult<usize> write(ReadonlyBytes bytes) override
        {
            VERIFY_NOT_REACHED();
        }

        VirtualFile& file() override { return m_file; }

        FlashFile& m_file;
        usize m_offset;
    };

    class FlashDirectory final : public VirtualDirectory {
    public:
        explicit FlashDirectory(FileInfo& info);

        VirtualFileHandle& create_handle() override;
    };

    // FIXME: This is redundant with MemoryDirectoryHandle
    class FlashDirectoryHandle final : public VirtualFileHandle {
    public:
        explicit FlashDirectoryHandle(FlashDirectory& directory)
            : m_iterator(directory.m_entries.iter())
            , m_directory(directory)
        {
        }

        KernelResult<usize> read(Bytes bytes) override
        {
            ASSERT(bytes.size() == sizeof(UserlandDirectoryInfo));

            if (m_iterator.begin() == m_iterator.end())
                return 0;

            auto& [name, file] = *m_iterator++;

            UserlandDirectoryInfo info;
            info.d_ino = file.must()->m_ino;
            name.strcpy_to({ info.d_name, sizeof(info.d_name) });
            return bytes_from(info).copy_to(bytes);
        }
        KernelResult<usize> write(ReadonlyBytes bytes) override
        {
            VERIFY_NOT_REACHED();
        }

        VirtualFile& file() override { return m_directory; }


        decltype(FlashDirectory::m_entries)::Iterator m_iterator;
        FlashDirectory& m_directory;
    };
}
