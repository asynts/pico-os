#include <Kernel/File.hpp>

namespace Kernel
{
    class RamFileHandle final : public VirtualFileHandle
    {
    public:
        using VirtualFileHandle::VirtualFileHandle;

        usize read(Bytes) override
        {
            FIXME();
        }

        usize write(ReadonlyBytes) override
        {
            FIXME();
        }
    };

    class FlashFileHandle final : public VirtualFileHandle
    {
    public:
        using VirtualFileHandle::VirtualFileHandle;

        usize read(Bytes) override
        {
            FIXME();
        }

        usize write(ReadonlyBytes) override
        {
            FIXME();
        }
    };

    class DeviceFileHandle final : public VirtualFileHandle {
    public:
        explicit DeviceFileHandle(File& file)
            : VirtualFileHandle(file)
        {
            m_device = Device::lookup(file.m_info.m_info->m_devno);
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
