#include <Kernel/FileSystem/DeviceFileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/ConsoleDevice.hpp>

namespace Kernel
{
    DeviceFileSystem::DeviceFileSystem()
    {
        MemoryFileSystem::the();

        // FIXME: Do we still need this?
        FileSystem::add_device(0x00010001, ConsoleFile::the());

        // FIXME: Figure this out
        // FileSystem::lookup("/dev").m_entries.set("tty", &ConsoleFile::the());
    }
}
