#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>

namespace Kernel
{
    extern "C" FileInfo __flash_root;

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

    FlashDirectory::FlashDirectory(FileInfo& info)
    {
        m_ino = info.st_ino;
        m_mode = info.st_mode;

        auto *begin = reinterpret_cast<const FlashDirectoryInfo*>(info.m_data);
        auto *end = reinterpret_cast<const FlashDirectoryInfo*>(info.m_data + info.st_size);

        for(auto *entry = begin; entry != end; ++entry)
            m_entries.set(entry->m_name, new FlashFile { *entry->m_info });
    }

    VirtualFileHandle& FlashDirectory::create_handle()
    {
        return *new FlashDirectoryHandle { *this };
    }
}
