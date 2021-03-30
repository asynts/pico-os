#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>

namespace Kernel
{
    Map<u32, Device*> Device::m_devices;

    ConsoleDevice::ConsoleDevice()
        : Device(1, 0)
    {
        auto& file = *new MemoryFile;
        file.m_device = device_id();
        file.m_mode = ModeFlags::DEVICE;
        file.m_size = 0;

        auto& directory_entry = *new MemoryDirectoryEntry;
        directory_entry.m_file = &file;

        auto& parent_dentry = FileSystem::lookup("/dev");
        parent_dentry.m_entries.append("tty", directory_entry);
    }
}
