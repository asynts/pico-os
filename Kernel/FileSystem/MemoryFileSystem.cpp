#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>

namespace Kernel
{
    MemoryDirectoryEntry::MemoryDirectoryEntry()
    {
        m_fs = &MemoryFileSystem::the();
        m_keep = true;
    }

    MemoryFileSystem::MemoryFileSystem()
    {
        auto& info = *new FileInfo;
        info.m_device = RAM_DEVICE_ID;
        info.m_devno = 0;
        info.m_direct_blocks[0] = nullptr;
        info.m_id = 2;
        info.m_mode = S_IFDIR;
        info.m_size = 0;

        m_root = new MemoryDirectoryEntry;
        m_root->m_info = &info;
        m_root->m_keep = true;
        m_root->m_loaded = true;
        m_root->m_name = "";
        m_root->m_entries.append(".", m_root);
        m_root->m_entries.append("..", m_root);

        m_root->add_entry("dev", create_directory());

        FileSystem::add_filesystem(RAM_DEVICE_ID);
    }
}
