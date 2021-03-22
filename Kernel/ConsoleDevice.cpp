#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>

namespace Kernel
{
    Map<u32, Device*> Device::m_devices;

    ConsoleDevice::ConsoleDevice()
        : Device(1, 0)
    {
        auto& info = MemoryFileSystem::the().create_device(device_id());

        MemoryFileSystem::the().lookup_path("/dev")
            .add_entry("tty", info, true);
    }
}
