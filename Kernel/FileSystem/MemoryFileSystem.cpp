#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>

namespace Kernel
{
    VirtualFile& MemoryFileSystem::create_file() { return *new MemoryFile; }
    VirtualFile& MemoryFileSystem::root() { return *m_root; }

    MemoryFileSystem::MemoryFileSystem()
    {
        m_root = new MemoryDirectory;
        ASSERT(m_root->m_ino == 2);

        m_root->m_entries.set(".", m_root);
        m_root->m_entries.set("..", m_root);

        auto& dev_file = *new MemoryFile;
        dev_file.m_device = 0;
        dev_file.m_mode = ModeFlags::Directory;

        m_root->m_entries.set("dev", &dev_file);
        m_root->m_entries.set("bin", &FlashFileSystem::the().root());
    }

    VirtualFileHandle& MemoryFile::create_handle()
    {
        auto& handle = *new MemoryFileHandle;
        handle.m_file = this;

        dbgln("[MemoryFile::create_handle] Created handle % for file % (ino=%)", &handle, this, m_ino);
        return handle;
    }

    VirtualFileHandle& MemoryDirectory::create_handle()
    {
        NOT_IMPLEMENTED();
    }
}
