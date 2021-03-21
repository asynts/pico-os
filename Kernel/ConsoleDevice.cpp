#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/MemoryFileSystem.hpp>

namespace Kernel {
    ConsoleDevice::ConsoleDevice()
        : Device(1, 0)
    {
        auto& info = MemoryFileSystem::the().create_device(device_id());

        MemoryFileSystem::the().lookup_path("/dev")
            .add_entry("tty", info, true);
    }
}
