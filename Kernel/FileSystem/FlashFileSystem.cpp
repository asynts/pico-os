#include <Kernel/FileSystem/FlashFileSystem.hpp>

// FIXME: Remove redundant fields
// FIXME: Move into interface
struct FlashFileInfo {
    u32 m_ino;
    u32 m_device;
    u32 m_mode;
    u32 m_size;
    u32 m_devno;
    u8 *m_data;
};
struct FlashDirectoryEntryInfo {
    char m_name[252];
    FlashFileInfo *m_info;
};

extern "C" FlashFileInfo __flash_root;

namespace Kernel
{
    VirtualFile& FlashFileSystem::create_file() { return *new FlashFile; }
    VirtualDirectoryEntry& FlashFileSystem::create_directory_entry() { return *new FlashDirectoryEntry; }
    VirtualDirectoryEntry& FlashFileSystem::root() { return *m_root; }

    VirtualFileHandle& FlashFileSystem::create_file_handle(VirtualFile& file)
    {
        // FIXME: This is a bit ugly
        auto& handle = *new FlashFileHandle;
        handle.m_file = dynamic_cast<FlashFile*>(&file);
        return handle;
    }

    FlashFileSystem::FlashFileSystem()
    {
        auto& root_file = *new FlashFile;
        root_file.m_ino = 2;
        root_file.m_mode = ModeFlags::Directory;
        root_file.m_size = 0;
        root_file.m_data = { __flash_root.m_data, __flash_root.m_size };

        auto& root_directory_entry = *new FlashDirectoryEntry;
        root_directory_entry.m_file = &root_file;

        m_root = &root_directory_entry;
    }

    void FlashDirectoryEntry::load()
    {
        auto *begin = reinterpret_cast<const FlashDirectoryEntryInfo*>(m_file->m_data.data());
        auto *end = reinterpret_cast<const FlashDirectoryEntryInfo*>(m_file->m_data.data() + m_file->m_data.size());

        for (auto *entry = begin; entry < end; ++entry)
        {
            // FIXME: Deal with hardlinks
            auto& file = *new FlashFile;
            file.m_data = { entry->m_info->m_data, entry->m_info->m_size };
            file.m_device = entry->m_info->m_devno;
            file.m_ino = entry->m_info->m_ino;
            file.m_mode = static_cast<ModeFlags>(entry->m_info->m_mode);
            file.m_size = entry->m_info->m_size;

            auto& dentry = *new FlashDirectoryEntry;
            dentry.m_file = &file;

            m_entries.append(entry->m_name, &dentry);
        }
    }
}
