#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>
#include <Kernel/Interface/stat.h>

namespace Kernel
{
    using namespace Std;

    class FlashFileSystem;
    class FlashFile;
    class FlashDirectory;
    class FlashFileHandle;

    struct FlashFileInfo;
    struct FlashDirectoryEntryInfo;

    // FIXME: Remove redundant fields
    // FIXME: Move into interface
    struct FlashFileInfo {
        u32 m_ino;
        u32 m_device;
        u32 m_mode;
        u32 m_size;
        u32 m_devno;
        u8 *m_data;
    };
    struct FlashDirectoryEntryInfo {
        char m_name[252];
        FlashFileInfo *m_info;
    };

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
        explicit FlashFile(FlashFileInfo& info)
            : m_data(info.m_data, info.m_size)
        {
            // FIXME: Verify that this is a file
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

        FlashFile& m_file;
        usize m_offset;
    };

    class FlashDirectory final : public VirtualDirectory {
    public:
        explicit FlashDirectory(FlashFileInfo& info);

        VirtualFileHandle& create_handle() override;
    };

    // FIXME: This is redundant with MemoryDirectoryHandle
    class FlashDirectoryHandle final : public VirtualFileHandle {
    public:
        explicit FlashDirectoryHandle(FlashDirectory& directory)
            : m_iterator(directory.m_entries.iter())
        {
        }

        KernelResult<usize> read(Bytes bytes) override
        {
            ASSERT(bytes.size() == sizeof(UserlandDirectoryInfo));

            if (m_iterator.begin() == m_iterator.end())
                return 0;

            auto& [name, file] = *m_iterator++;

            UserlandDirectoryInfo info;
            name.strcpy_to({ info.d_name, sizeof(info.d_name) });
            return bytes_from(info).copy_to(bytes);
        }
        KernelResult<usize> write(ReadonlyBytes bytes) override
        {
            VERIFY_NOT_REACHED();
        }

        decltype(FlashDirectory::m_entries)::Iterator m_iterator;
    };
}
