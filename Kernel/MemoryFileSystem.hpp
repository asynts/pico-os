#pragma once

#include <Std/Forward.hpp>
#include <Std/String.hpp>
#include <Std/Map.hpp>
#include <Std/Singleton.hpp>
#include <Std/Format.hpp>

#include <Kernel/VirtualFileSystem.hpp>
#include <Kernel/Interface/vfs.h>

namespace Kernel
{
    using namespace Std;

    struct MemoryDirectoryEntry final : VirtualDirectoryEntry
    {
        MemoryDirectoryEntry();

        void load_directory_entries() override
        {
            m_loaded = true;
        }
    };

    class MemoryFileSystem final
        : public Singleton<MemoryFileSystem>
        , public VirtualFileSystem
    {
    public:
        VirtualDirectoryEntry& root() override { return *m_root; }

        VirtualDirectoryEntry& create_empty_dentry() override
        {
            return *new MemoryDirectoryEntry;
        }

        FileInfo& create_directory()
        {
            auto& info = *new FileInfo;
            info.m_device = RAM_DEVICE_ID;
            info.m_direct_blocks[0] = nullptr;
            info.m_id = m_next_ino++;
            info.m_mode = S_IFDIR;
            info.m_size = 0;

            return info;
        }

        FileInfo& create_device(u32 device_id)
        {
            auto& info = *new FileInfo;
            info.m_device = RAM_DEVICE_ID;
            info.m_direct_blocks[0] = nullptr;
            info.m_id = m_next_ino++;
            info.m_mode = S_IFDEV;
            info.m_size = 0;
            info.m_devno = device_id;

            return info;
        }

    private:
        friend Singleton<MemoryFileSystem>;
        MemoryFileSystem()
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
        }

        MemoryDirectoryEntry *m_root;
        u32 m_next_ino = 3;
    };
}
