#pragma once

#include <Std/Singleton.hpp>

#include <Kernel/FileSystem/VirtualFileSystem.hpp>

namespace Kernel
{
    struct FlashDirectoryEntry final : VirtualDirectoryEntry
    {
        FlashDirectoryEntry();

        void load_directory_entries() override
        {
            if (m_loaded)
                return;
            m_loaded = true;

            auto *begin = reinterpret_cast<FlashDirectoryEntryInfo*>(m_info->m_direct_blocks[0]);
            auto *end = reinterpret_cast<FlashDirectoryEntryInfo*>(m_info->m_direct_blocks[0] + m_info->m_size);

            for (auto *entry = begin; entry < end; ++entry)
                add_entry(entry->m_name, *entry->m_info);
        }
    };

    class FlashFileSystem final
        : public Singleton<FlashFileSystem>
        , public VirtualFileSystem
    {
    public:
        VirtualDirectoryEntry& root() override { return *m_root; }

        VirtualDirectoryEntry& create_empty_dentry() override
        {
            return *new FlashDirectoryEntry;
        }

    private:
        friend Singleton<FlashFileSystem>;
        FlashFileSystem();

        VirtualDirectoryEntry *m_root;
    };
}
