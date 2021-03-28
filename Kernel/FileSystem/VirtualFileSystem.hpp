#pragma once

#include <Std/Forward.hpp>
#include <Std/String.hpp>
#include <Std/Map.hpp>
#include <Std/Path.hpp>

#include <Kernel/Interface/vfs.h>

namespace Kernel
{
    using namespace Std;

    struct VirtualFileSystem;

    struct VirtualDirectoryEntry {
        String m_name;
        FileInfo *m_info;
        VirtualFileSystem *m_fs;
        bool m_keep;
        Map<String, VirtualDirectoryEntry*> m_entries;
        bool m_loaded = false;

        virtual void load_directory_entries() = 0;

        VirtualDirectoryEntry& add_entry(StringView name, FileInfo& info, bool keep = false);
    };

    class VirtualFileSystem {
    public:
        virtual ~VirtualFileSystem() = default;

        virtual VirtualDirectoryEntry& root() = 0;
        virtual VirtualDirectoryEntry& create_empty_dentry() = 0;

        VirtualDirectoryEntry& lookup_path(Path path)
        {
            dbgln("[VirtualFileSystem::lookup_path] path=%", path);

            VirtualDirectoryEntry *info = &root();
            for (auto& component : path.components())
            {
                info->load_directory_entries();
                dbgln("[VirtualFileSystem::lookup_path] component=%", component);
                info = info->m_entries.lookup(component).must();
            }
            return *info;
        }
    };
}
