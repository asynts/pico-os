#pragma once

#include <Kernel/FileSystem/VirtualFileSystem.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/Interface/stat.h>

namespace Kernel
{
    class VirtualFileHandle;

    class VirtualFile {
    public:
        virtual ~VirtualFile() = default;

        explicit VirtualFile(FileInfo& info)
            : m_info(info)
        {
        }

        virtual VirtualFileHandle& create_handle() = 0;

        FileInfo& m_info;
    };

    class VirtualFileHandle {
    public:
        virtual ~VirtualFileHandle() = default;

        virtual usize read(Bytes) = 0;
        virtual usize write(ReadonlyBytes) = 0;

        virtual FileInfo& info() = 0;
    };

    class RamFile final : public VirtualFile {
    public:
        using VirtualFile::VirtualFile;

        VirtualFileHandle& create_handle() override;
    };
    class RamFileHandle final : public VirtualFileHandle
    {
    public:
        RamFileHandle(RamFile& file)
            : m_file(file)
            , m_offset(0)
        {
        }

        usize read(Bytes bytes) override
        {
            if ((m_file.m_info.m_mode & S_IFMT) == S_IFDIR) {
                // Oops. We don't have access to the dentry here
                NOT_IMPLEMENTED();
            } else {
                usize nread = ReadonlyBytes {
                    m_file.m_info.m_direct_blocks[0] + m_offset,
                    m_file.m_info.m_size - m_offset,
                }.copy_trimmed_to(bytes);

                m_offset += nread;
                return nread;
            }
        }

        usize write(ReadonlyBytes bytes) override
        {
            if (m_file.m_info.m_direct_blocks[0] == nullptr)
                m_file.m_info.m_direct_blocks[0] = new u8[RAM_BLOCK_SIZE];

            VERIFY(m_file.m_info.m_size + bytes.size() <= RAM_BLOCK_SIZE);

            bytes.copy_to({
                m_file.m_info.m_direct_blocks[0] + m_offset,
                RAM_BLOCK_SIZE - m_offset,
            });
            m_offset += bytes.size();
            m_file.m_info.m_size = max<u32>(m_file.m_info.m_size, m_offset);

            return bytes.size();
        }

        FileInfo& info() override
        {
            return m_file.m_info;
        }

    private:
        RamFile& m_file;
        usize m_offset;
    };

    class FlashFile final : public VirtualFile {
    public:
        FlashFile(FileInfo& info)
            : VirtualFile(info)
        {
            m_bytes = { m_info.m_direct_blocks[0], m_info.m_size };
        }

        VirtualFileHandle& create_handle() override;

        ReadonlyBytes m_bytes;
    };
    class FlashFileHandle final : public VirtualFileHandle
    {
    public:
        FlashFileHandle(FlashFile& file)
            : m_file(file)
            , m_offset(0)
        {
        }

        usize read(Bytes bytes) override
        {
            if ((m_file.m_info.m_mode & S_IFMT) == S_IFDIR) {
                FlashDirectoryEntryInfo entry;
                usize nread = m_file.m_bytes.slice(m_offset).copy_trimmed_to({ (u8*)&entry, sizeof(entry) });

                if (nread == 0)
                    return 0;

                VERIFY(nread == sizeof(FlashDirectoryEntryInfo));

                m_offset += nread;

                UserlandDirectoryInfo info;
                StringView { entry.m_name }.strcpy_to({ info.d_name, sizeof(info.d_name) });

                return ReadonlyBytes { (const u8*)&info, sizeof(info) }.copy_to(bytes);
            } else {
                usize nread = m_file.m_bytes.slice(m_offset).copy_trimmed_to(bytes);
                m_offset += nread;
                return nread;
            }
        }

        usize write(ReadonlyBytes) override
        {
            VERIFY_NOT_REACHED();
        }

        FileInfo& info() override
        {
            return m_file.m_info;
        }

    private:
        FlashFile& m_file;
        usize m_offset;
    };

    class DeviceFile final : public VirtualFile {
    public:
        DeviceFile(FileInfo& info)
            : VirtualFile(info)
        {
            VERIFY((info.m_mode & S_IFMT) == S_IFDEV);

            m_device = Device::lookup(m_info.m_devno);
            VERIFY(m_device);
        }

        VirtualFileHandle& create_handle() override;

        Device *m_device;
    };
    class DeviceFileHandle final : public VirtualFileHandle {
    public:
        explicit DeviceFileHandle(DeviceFile& file)
            : m_file(file)
            , m_offset(0)
        {
        }

        usize read(Bytes bytes) override
        {
            return m_file.m_device->read(bytes);
        }

        usize write(ReadonlyBytes bytes) override
        {
            return m_file.m_device->write(bytes);
        }

        FileInfo& info() override
        {
            return m_file.m_info;
        }

    private:
        DeviceFile& m_file;
        usize m_offset;
    };
}
