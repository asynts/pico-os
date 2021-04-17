#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>

namespace Kernel
{
    VirtualFile& MemoryFileSystem::root() { return *m_root; }

    MemoryFileSystem::MemoryFileSystem()
    {
        m_root = new MemoryDirectory;
        ASSERT(m_root->m_ino == 2);

        m_root->m_entries.set(".", m_root);
        m_root->m_entries.set("..", m_root);

        auto& dev_directory = *new MemoryDirectory;
        dev_directory.m_entries.set("..", m_root);
        m_root->m_entries.set("dev", &dev_directory);

        auto& bin_directory = dynamic_cast<VirtualDirectory&>(FlashFileSystem::the().root());
        bin_directory.m_entries.set("..", m_root);
        m_root->m_entries.set("bin", &bin_directory);
    }

    VirtualFileHandle& MemoryFile::create_handle()
    {
        return *new MemoryFileHandle { *this };
    }

    VirtualFileHandle& MemoryDirectory::create_handle()
    {
        return *new MemoryDirectoryHandle { *this };
    }
}
