#pragma once

#include <Std/Singleton.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel
{
    class DeviceFile;
    class DeviceFileHandle;
    class DeviceFileSystem;

    class DeviceFileSystem final
        : public Singleton<DeviceFileSystem>
        , public VirtualFileSystem
    {
    public:
        // FIXME: Looks like I choose the wrong abstractions
        VirtualFile& root() override { VERIFY_NOT_REACHED(); }

        void add_device(u32 device_id, VirtualFile& file)
        {
            m_devices.set(device_id, &file);
        }

        VirtualFileHandle& create_device_handle(u32 device_id)
        {
            return m_devices.get_opt(device_id).must()->create_handle();
        }

    private:
        HashMap<u32, VirtualFile*> m_devices;

        friend Singleton<DeviceFileSystem>;
        DeviceFileSystem();
    };
}
