#include <Kernel/FileSystem/DeviceFileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/ConsoleDevice.hpp>

namespace Kernel
{
    DeviceFileSystem::DeviceFileSystem()
    {
        MemoryFileSystem::the();

        auto& dev_file = FileSystem::lookup("/dev");
        auto& dev_directory = dynamic_cast<VirtualDirectory&>(dev_file);

        // FIXME: Move this to ConsoleFile::ConsoleFile
        add_device(0x00010001, ConsoleFile::the());
        auto& tty_file = *new MemoryFile;
        tty_file.m_mode = ModeFlags::Device;
        tty_file.m_device_id = 0x00010001;
        dev_directory.m_entries.set("tty", &tty_file);
    }
}
