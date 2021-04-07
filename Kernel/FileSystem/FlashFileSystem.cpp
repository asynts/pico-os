#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>

namespace Kernel
{
    extern "C" FlashFileInfo __flash_root;

    VirtualFile& FlashFileSystem::root() { return *m_root; }

    FlashFileSystem::FlashFileSystem()
    {
        m_root = new FlashDirectory { __flash_root };
        VERIFY(m_root->m_ino == 2);
    }

    VirtualFileHandle& FlashFile::create_handle()
    {
        return *new FlashFileHandle { *this };
    }

    FlashDirectory::FlashDirectory(FlashFileInfo& info)
    {
        m_ino = info.m_ino;
        m_mode = ModeFlags::Directory;

        auto *begin = reinterpret_cast<const FlashDirectoryEntryInfo*>(info.m_data);
        auto *end = reinterpret_cast<const FlashDirectoryEntryInfo*>(info.m_data + info.m_size);

        for(auto *entry = begin; entry != end; ++entry)
            m_entries.set(entry->m_name, new FlashFile { *entry->m_info });
    }

    VirtualFileHandle& FlashDirectory::create_handle()
    {
        return *new FlashDirectoryHandle { *this };
    }
}
