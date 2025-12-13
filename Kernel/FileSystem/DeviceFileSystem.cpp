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

        // Use ConsoleFile directly so stat() returns correct device info
        dev_directory.m_entries.set("tty", &ConsoleFile::the());
    }
}
