#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem.hpp>

namespace Kernel {
    ConsoleDevice::ConsoleDevice()
        : Device(1, 0)
    {
        auto& info = VirtualFileSystem::the().create_ram_device(device_id());

        VirtualFileSystem::the().lookup_path("/dev")
            .add_entry("tty", info, true);
    }
}
