#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>

namespace Kernel
{
    VirtualFile& MemoryFileSystem::create_file() { return *new MemoryFile; }
    VirtualDirectoryEntry& MemoryFileSystem::create_directory_entry() { return *new MemoryDirectoryEntry; }
    VirtualDirectoryEntry& MemoryFileSystem::root() { return *m_root; }

    MemoryFileSystem::MemoryFileSystem()
    {
        auto& root_file = *new MemoryFile;
        root_file.m_ino = 2;
        root_file.m_mode = ModeFlags::Directory;
        root_file.m_size = 0;

        auto& root_directory_entry = *new MemoryDirectoryEntry;
        root_directory_entry.m_file = &root_file;
        root_directory_entry.m_loaded = true;
        root_directory_entry.m_entries.append(".", &root_directory_entry);
        root_directory_entry.m_entries.append("..", &root_directory_entry);

        m_root = &root_directory_entry;

        auto& dev_file = *new MemoryFile;
        dev_file.m_device = 0;
        dev_file.m_mode = ModeFlags::Directory;
        auto& dev_dentry = *new MemoryDirectoryEntry;
        dev_dentry.m_file = &dev_file;
        root_directory_entry.m_entries.append("dev", &dev_dentry);

        auto& bin_dentry = FlashFileSystem::the().root();
        root_directory_entry.m_entries.append("bin", &bin_dentry);

        m_next_ino = 3;
    }

    VirtualFileHandle& MemoryFile::create_handle()
    {
        auto& handle = *new MemoryFileHandle;
        handle.m_file = this;
        return handle;
    }
}
