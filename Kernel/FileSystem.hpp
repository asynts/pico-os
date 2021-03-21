#pragma once

#include <Std/Forward.hpp>
#include <Std/String.hpp>
#include <Std/Map.hpp>
#include <Std/Singleton.hpp>
#include <Std/Format.hpp>

#include <Kernel/Interface/vfs.h>

namespace Kernel
{
    using namespace Std;

    template<typename Callback>
    void iterate_path_components(StringView path, Callback&& callback)
    {
        assert(path.size() >= 1);
        assert(path[0] == '/');
        path = path.substr(1);

        if (path.size() == 0)
            return;

        auto end_index = path.index_of('/');

        if (!end_index.is_valid()) {
            callback(path, true);
            return;
        } else {
            if (callback(path.trim(end_index.value()), false) == IterationDecision::Break)
                return;
            return iterate_path_components(path.substr(end_index.value()), move(callback));
        }
    }

    inline StringBuilder compute_absolute_path(StringView path)
    {
        // FIXME: Introduce process seperation
        StringView current_working_directory = "/";

        StringBuilder builder;
        builder.append(current_working_directory);
        builder.append(path);

        return builder;
    }

    extern "C" FileInfo __flash_root;

    struct DirectoryEntryInfo {
        String m_name;
        FileInfo *m_info;
        bool m_keep;
        Map<String, DirectoryEntryInfo*> m_entries;
        bool m_loaded = false;

        void load_directory_entries()
        {
            dbgln("Loading directory entry info for inode=% device=% name=%", m_info->m_id, m_info->m_device, m_name);

            if (m_loaded)
                return;
            m_loaded = true;

            if (m_info->m_device == RAM_DEVICE_ID)
                return;

            if (m_info->m_device == FLASH_DEVICE_ID) {
                auto *begin = reinterpret_cast<FlashDirectoryEntryInfo*>(m_info->m_direct_blocks[0]);
                auto *end = reinterpret_cast<FlashDirectoryEntryInfo*>(m_info->m_direct_blocks[0] + m_info->m_size);

                dbgln("Loading % directory entries from inode=% device=%", end - begin, m_info->m_id, m_info->m_device);

                for (auto *entry = begin; begin < end; ++begin) {
                    auto *new_info = new DirectoryEntryInfo;
                    new_info->m_name = entry->m_name;
                    new_info->m_info = entry->m_info;
                    new_info->m_keep = false;
                    new_info->m_entries.append(".", new_info);
                    new_info->m_entries.append("..", this);
                    m_entries.append(entry->m_name, new_info);

                    dbgln("(Repeated) Loading directory with from entry=% info=%", entry, entry->m_info);
                    dbgln("Loaded directory entry info for inode=% name=%", entry->m_info->m_id, entry->m_name);
                }
                return;
            }

            dbgln("DirectoryEntryInfo::load_directory_entries Unknown device %", m_info->m_device);
            assert(false);
        }

        DirectoryEntryInfo& add_entry(StringView name, FileInfo &info, bool keep = false)
        {
            if (keep)
                assert(m_keep);

            auto& dentry = *new DirectoryEntryInfo;
            dentry.m_name = name;
            dentry.m_info = &info;
            dentry.m_keep = keep;
            dentry.m_entries.append(".", &dentry);
            dentry.m_entries.append("..", this);

            m_entries.append(name, &dentry);

            return dentry;
        }
    };

    class VirtualFileSystem : public Singleton<VirtualFileSystem> {
    public:
        DirectoryEntryInfo& lookup_path(StringView path)
        {
            dbgln("VirtualFileSystem::lookup_path path=%", path);

            DirectoryEntryInfo *info = m_root_dentry_info;
            iterate_path_components(path, [&](StringView component, bool final) {
                dbgln("VirtualFileSystem::lookup_path component=% final=% in inode=% device=%", component, final, info->m_info->m_id, info->m_info->m_device);
                info->load_directory_entries();
                info = info->m_entries.lookup(component).must();
                dbgln("VirtualFileSystem::lookup_path found % as inode=% device=%", component, info->m_info->m_id, info->m_info->m_device);
                return IterationDecision::Continue;
            });
            return *info;
        }

        FileInfo& create_ram_directory()
        {
            auto& info = *new FileInfo;
            info.m_device = RAM_DEVICE_ID;
            info.m_direct_blocks[0] = nullptr;
            info.m_id = m_next_ram_id++;
            info.m_mode = S_IFDIR;
            info.m_size = 0;

            return info;
        }

        FileInfo& create_ram_device(u32 device_id)
        {
            auto& info = *new FileInfo;
            info.m_device = RAM_DEVICE_ID;
            info.m_direct_blocks[0] = nullptr;
            info.m_id = m_next_ram_id++;
            info.m_mode = S_IFDEV;
            info.m_size = 0;
            info.m_devno = device_id;

            return info;
        }

    private:
        friend Singleton<VirtualFileSystem>;
        VirtualFileSystem()
        {
            m_root_file_info = new FileInfo;
            m_root_file_info->m_id = 2;
            m_root_file_info->m_device = RAM_DEVICE_ID;
            m_root_file_info->m_mode = S_IFDIR;
            m_root_file_info->m_size = 0;
            m_root_file_info->m_direct_blocks[0] = nullptr;

            m_root_dentry_info = new DirectoryEntryInfo;
            m_root_dentry_info->m_name = "";
            m_root_dentry_info->m_info = m_root_file_info;
            m_root_dentry_info->m_keep = true;
            m_root_dentry_info->m_entries.append(".", m_root_dentry_info);
            m_root_dentry_info->m_entries.append("..", m_root_dentry_info);

            m_root_dentry_info->add_entry("bin", __flash_root, true);
            m_root_dentry_info->add_entry("dev", create_ram_directory(), true);
        }

        FileInfo *m_root_file_info;
        DirectoryEntryInfo *m_root_dentry_info;

        u32 m_next_ram_id = 3;
    };
}
