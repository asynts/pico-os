#pragma once

#include <Std/Forward.hpp>
#include <Std/String.hpp>
#include <Std/Vector.hpp>
#include <Std/Map.hpp>
#include <Std/StringBuilder.hpp>

namespace Std
{
    enum class IterationDecision {
        Continue,
        Break,
    };
}

// In this filesystem `m_direct_blocks[0]` is null and `DirectoryEntryInfo`s are generated
// with `m_keep` set.
#define DEVICE_RAM 1

// In this filesystem `m_direct_blocks[0]` is a pointer to the data which is stored
// contiguously.
//
// Directories are encoded as normal files where the data is an array of
// `FlashDirectoryEntryInfo`s which can be used to generate `DirectoryEntryInfo`s.
#define DEVICE_FLASH 2

#define S_IFDIR 1
#define S_IFREG 2

namespace Kernel
{
    using namespace Std;

    template<typename Callback>
    void iterate_path_components(StringView path, Callback&& callback);

    struct FileInfo {
        u32 m_id;
        u32 m_device;
        u32 m_mode;

        u8 *m_direct_blocks[1];
        u8 **m_indirect_blocks[4];
    };
    struct FlashDirectoryEntryInfo {
        char m_name[252];
        FileInfo *m_info;
    };
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

            if (m_info->m_device == DEVICE_RAM)
                return;

            if (m_info->m_device == DEVICE_FLASH) {
                auto *begin = reinterpret_cast<FlashDirectoryEntryInfo*>(m_info->m_direct_blocks[0]);
                auto *end = reinterpret_cast<FlashDirectoryEntryInfo*>(m_info->m_direct_blocks[0] + m_info->m_size);

                for (auto *entry = begin; begin < end; ++begin) {
                    StringBuilder name_builder;
                    name_builder.append('/');
                    name_builder.append(entry->m_name);

                    auto *new_info = new DirectoryEntryInfo;
                    new_info->m_name = name_builder.view();
                    new_info->m_info = entry->m_info;
                    new_info->m_keep = false;
                    new_info->m_entries.append(".", new_info);
                    new_info->m_entries.append("..", this);
                    m_entries.append(entry->m_name, new_info);
                }
                return;
            }

            assert(false);
        }
    };

    extern "C" FileInfo __flash_root;

    class VirtualFileSystem {
    public:
        VirtualFileSystem()
        {
            m_root_file_info = new FileInfo;
            m_root_file_info->m_id = 2;
            m_root_file_info->m_device = DEVICE_RAM;
            m_root_file_info->m_mode = S_IFDIR;
            m_root_file_info->m_direct_blocks[0] = nullptr;

            m_root_dentry_info = new DirectoryEntryInfo;
            m_root_dentry_info->m_name = "/";
            m_root_dentry_info->m_info = m_root_file_info;
            m_root_dentry_info->m_keep = true;
            m_root_dentry_info->m_entries.append(".", m_root_dentry_info);
            m_root_dentry_info->m_entries.append("..", m_root_dentry_info);

            DirectoryEntryInfo *bin_info = new DirectoryEntryInfo;
            bin_info->m_name = "/bin";
            bin_info->m_info = &__flash_root;
            bin_info->m_keep = true;
            bin_info->m_entries.append(".", bin_info);
            bin_info->m_entries.append("..", m_root_dentry_info);
            m_root_dentry_info->m_entries.append("bin", bin_info);
        }

        DirectoryEntryInfo& lookup_path(StringView path)
        {
            DirectoryEntryInfo *info = m_root_dentry_info;
            iterate_path_components(path, [&](StringView component, bool final) {
                info->load_directory_entries();
                info = info->m_entries.lookup(component).must();
            });
            return *info;
        }

    private:
        FileInfo *m_root_file_info;
        DirectoryEntryInfo *m_root_dentry_info;
    };
}
