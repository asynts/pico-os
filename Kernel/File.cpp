#include <Kernel/File.hpp>

namespace Kernel
{
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
            usize nread = ReadonlyBytes {
                m_file.m_info.m_direct_blocks[0] + m_offset,
                m_file.m_info.m_size - m_offset,
            }.copy_trimmed_to(bytes);

            m_offset += nread;
            return nread;
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

    private:
        RamFile& m_file;
        usize m_offset;
    };
    VirtualFileHandle& RamFile::create_handle()
    {
        return *new RamFileHandle { *this };
    }

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
            usize nread = m_file.m_bytes.slice(m_offset).copy_trimmed_to(bytes);
            m_offset += nread;
            return nread;
        }

        usize write(ReadonlyBytes) override
        {
            VERIFY_NOT_REACHED();
        }

    private:
        FlashFile& m_file;
        usize m_offset;
    };
    VirtualFileHandle& FlashFile::create_handle()
    {
        return *new FlashFileHandle { *this };
    }

    class DeviceFile final : public VirtualFile {
    public:
        DeviceFile(FileInfo& info)
            : VirtualFile(info)
        {
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

    private:
        DeviceFile& m_file;
        usize m_offset;
    };
    VirtualFileHandle& DeviceFile::create_handle()
    {
        return *new DeviceFileHandle { *this };
    }
}
