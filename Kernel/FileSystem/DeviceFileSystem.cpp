#include <Kernel/FileSystem/DeviceFileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/ConsoleDevice.hpp>

namespace Kernel
{
    DeviceFileSystem::DeviceFileSystem()
    {
        MemoryFileSystem::the();

        FileSystem::add_device(0x00010001, ConsoleFile::the());

        auto& console_dentry = *new MemoryDirectoryEntry;
        console_dentry.m_file = &ConsoleFile::the();

        FileSystem::lookup("/dev").m_entries.append("tty", &console_dentry);
    }
}
