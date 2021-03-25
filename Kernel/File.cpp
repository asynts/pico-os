#include <Kernel/File.hpp>

namespace Kernel
{
    class RamFileHandle final : public VirtualFileHandle
    {
    public:
        using VirtualFileHandle::VirtualFileHandle;

        usize read(Bytes bytes) override
        {
            return ReadonlyBytes {
                m_file.m_info.m_info->m_direct_blocks[0] + m_file.m_offset,
                m_file.m_info.m_info->m_size - m_file.m_offset,
            }.copy_trimmed_to(bytes);
        }

        usize write(ReadonlyBytes bytes) override
        {
            if (m_file.m_info.m_info->m_direct_blocks[0] == nullptr)
                m_file.m_info.m_info->m_direct_blocks[0] = new u8[RAM_BLOCK_SIZE];

            assert(m_file.m_info.m_info->m_size + bytes.size() <= RAM_BLOCK_SIZE);

            bytes.copy_to({
                m_file.m_info.m_info->m_direct_blocks[0] + m_file.m_offset,
                RAM_BLOCK_SIZE - m_file.m_offset,
            });
            m_file.m_offset += bytes.size();
            m_file.m_info.m_info->m_size = max<u32>(m_file.m_info.m_info->m_size, m_file.m_offset);

            return bytes.size();
        }
    };

    class FlashFileHandle final : public VirtualFileHandle
    {
    public:
        FlashFileHandle(File& file)
            : VirtualFileHandle(file)
        {
            m_bytes = { file.m_info.m_info->m_direct_blocks[0], file.m_info.m_info->m_size };
        }

        using VirtualFileHandle::VirtualFileHandle;

        usize read(Bytes bytes) override
        {
            dbgln("FlashFileHandle::read about to copy");

            usize nread = m_bytes.slice(m_offset).copy_trimmed_to(bytes);
            m_offset += nread;

            dbgln("FlashFileHandle::read copied");

            dbgln("FlashFileHandle::read nread=%", nread);

            return nread;
        }

        usize write(ReadonlyBytes) override
        {
            ASSERT_NOT_REACHED();
        }

    private:
        ReadonlyBytes m_bytes;
        usize m_offset = 0;
    };

    class DeviceFileHandle final : public VirtualFileHandle {
    public:
        explicit DeviceFileHandle(File& file)
            : VirtualFileHandle(file)
        {
            m_device = Device::lookup(file.m_info.m_info->m_devno);
            assert(m_device);
        }

        usize read(Bytes bytes) override
        {
            return m_device->read(bytes);
        }

        usize write(ReadonlyBytes bytes) override
        {
            return m_device->write(bytes);
        }

    private:
        Device *m_device;
    };

    VirtualFileHandle& File::create_handle()
    {
        dbgln("File::create_handle: mode=% (S_IFDEV=%)", m_info.m_info->m_mode, S_IFDEV);

        if ((m_info.m_info->m_mode & S_IFMT) == S_IFDEV) {
            dbgln("Creating DeviceFileHandle for inode=% device=%", m_info.m_info->m_id, m_info.m_info->m_device);
            return *new DeviceFileHandle { *this };
        }

        if (m_info.m_info->m_device == RAM_DEVICE_ID) {
            dbgln("Creating RamFileHandle for inode=% device=%", m_info.m_info->m_id, m_info.m_info->m_device);
            return *new RamFileHandle { *this };
        } if (m_info.m_info->m_device == FLASH_DEVICE_ID) {
            dbgln("Creating FlashFileHandle for inode=% device=%", m_info.m_info->m_id, m_info.m_info->m_device);
            return *new FlashFileHandle { *this };
        }

        ASSERT_NOT_REACHED();
    }
}
