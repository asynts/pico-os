#pragma once

#include <Std/Forward.hpp>
#include <Std/String.hpp>
#include <Std/Map.hpp>
#include <Std/Singleton.hpp>

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

    struct DirectoryEntryInfo {
        String m_name;
        FileInfo *m_info;
        bool m_keep;
        Map<String, DirectoryEntryInfo*> m_entries;
        bool m_loaded = false;

        void load_directory_entries()
        {
            if (m_loaded)
                return;
            m_loaded = true;

            if (m_info->m_device == RAM_DEVICE_ID)
                return;

            if (m_info->m_device == FLASH_DEVICE_ID) {
                auto *begin = reinterpret_cast<FlashDirectoryEntryInfo*>(m_info->m_direct_blocks[0]);
                auto *end = reinterpret_cast<FlashDirectoryEntryInfo*>(m_info->m_direct_blocks[0] + m_info->m_size);

                for (auto *entry = begin; begin < end; ++begin) {
                    auto *new_info = new DirectoryEntryInfo;
                    new_info->m_name = entry->m_name;
                    new_info->m_info = entry->m_info;
                    new_info->m_keep = false;
                    new_info->m_entries.append(".", new_info);
                    new_info->m_entries.append("..", this);
                    m_entries.append(entry->m_name, new_info);
                }
                return;
            }

            dbgln("DirectoryEntryInfo::load_directory_entries Unknown device %", m_info->m_device);
            assert(false);
        }
    };

    extern "C" FileInfo __flash_root;

    class VirtualFileSystem : public Singleton<VirtualFileSystem> {
    public:
        DirectoryEntryInfo& lookup_path(StringView path)
        {
            DirectoryEntryInfo *info = m_root_dentry_info;
            iterate_path_components(path, [&](StringView component, bool final) {
                info->load_directory_entries();
                info = info->m_entries.lookup(component).must();
                return IterationDecision::Continue;
            });
            return *info;
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
            m_root_dentry_info->m_name = "/";
            m_root_dentry_info->m_info = m_root_file_info;
            m_root_dentry_info->m_keep = true;
            m_root_dentry_info->m_entries.append(".", m_root_dentry_info);
            m_root_dentry_info->m_entries.append("..", m_root_dentry_info);

            DirectoryEntryInfo *bin_info = new DirectoryEntryInfo;
            bin_info->m_name = "bin";
            bin_info->m_info = &__flash_root;
            bin_info->m_keep = true;
            bin_info->m_entries.append(".", bin_info);
            bin_info->m_entries.append("..", m_root_dentry_info);
            m_root_dentry_info->m_entries.append("bin", bin_info);
        }

        FileInfo *m_root_file_info;
        DirectoryEntryInfo *m_root_dentry_info;
    };
}
