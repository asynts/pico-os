#include <Kernel/FileSystem/MemoryFileSystem.hpp>

namespace Kernel
{
    VirtualFile& MemoryFileSystem::create_file() { return *new MemoryFile; }
    VirtualDirectoryEntry& MemoryFileSystem::create_directory_entry() { return *new MemoryDirectoryEntry; }
    VirtualDirectoryEntry& MemoryFileSystem::root() { return *m_root; }

    VirtualFileHandle& MemoryFileSystem::create_file_handle(VirtualFile& file)
    {
        // FIXME: This is a bit ugly
        auto& handle = *new MemoryFileHandle;
        handle.m_file = dynamic_cast<MemoryFile*>(&file);
        return handle;
    }

    MemoryFileSystem::MemoryFileSystem()
    {
        auto& root_file = *new MemoryFile;
        root_file.m_ino = 2;
        root_file.m_mode = ModeFlags::DIRECTORY;
        root_file.m_size = 0;

        auto& root_directory_entry = *new MemoryDirectoryEntry;
        root_directory_entry.m_file = &root_file;
        root_directory_entry.m_loaded = true;
        root_directory_entry.m_entries.append(".", &root_directory_entry);
        root_directory_entry.m_entries.append("..", &root_directory_entry);

        m_root = &root_directory_entry;

        m_next_ino = 3;
    }
}
