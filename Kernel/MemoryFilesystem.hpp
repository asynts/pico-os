#pragma once

#include <Std/Singleton.hpp>
#include <Std/StringView.hpp>

#define S_IFMT  0b11
#define S_IFCHR 0b01
#define S_IFDIR 0b10
#define S_IFREG 0b11

// FIXME: This is replicated in Tools/FileSystem.cpp
struct IndexNode {
    uint32_t m_inode;
    uint32_t m_mode;
    uint32_t m_size;
    uint32_t m_device_id;
    uint32_t m_block_size;
    u8* m_direct_blocks[1];
    u8** m_indirect_blocks[4];
};
struct FlashEntry {
    char m_path[252];
    IndexNode *m_inode;
};
static_assert(sizeof(FlashEntry) == 256);

extern "C"
{
    extern const FlashEntry __embed_start[];
    extern const FlashEntry __embed_end[];
}

namespace Kernel {
    using namespace Std;

    struct DirectoryEntry {
        u32 m_inode;
        char m_name[256 - sizeof(m_inode)];

        StringView name() const { return m_name; }
    };

    constexpr usize block_size = 0x1000;
    constexpr usize directory_entries_per_block = block_size / sizeof(DirectoryEntry);
    static_assert(block_size % sizeof(DirectoryEntry) == 0);

    constexpr u32 device_invalid = 0;
    constexpr u32 device_ram = 1;
    constexpr u32 device_flash = 2;

    struct File {
        u32 m_inode;
        u32 m_mode;
        u32 m_size;
        u32 m_device_id;
        u8 *m_direct_blocks[1];
        u8 **m_indirect_blocks[4];

        bool is_directory() const
        {
            return (m_mode & S_IFMT) == S_IFDIR;
        }

        usize directory_size() const
        {
            assert(is_directory());
            assert(m_size % sizeof(DirectoryEntry) == 0);
            return m_size / sizeof(DirectoryEntry);
        }

        void append_file(const char *name, File& file)
        {
            assert(is_directory());
            assert(directory_size() <= directory_entries_per_block);

            auto *entries = reinterpret_cast<DirectoryEntry*>(m_direct_blocks);

            usize index = directory_size();
            m_size += sizeof(DirectoryEntry);

            dbgln("Inserting inode % (%) into inode %", file.m_inode, name, m_inode);

            strlcpy(entries[index].m_name, name, sizeof(entries[index].m_name));
            entries[index].m_inode = file.m_inode;
        }
    };

    enum class IterationDecision {
        Continue,
        Break,
    };

    template<typename Callback>
    void iterate_split_path(StringView path, Callback&& callback)
    {
        assert(path.size() >= 1);
        assert(path[0] == '/');
        path = path.substr(1);

        auto end_index = path.index_of('/');

        if (!end_index.is_valid()) {
            callback(path, true);
            return;
        } else {
            if (callback(path.trim(end_index.value()), false) == IterationDecision::Break)
                return;
            return iterate_split_path(path.substr(end_index.value()), move(callback));
        }
    }

    template<typename Callback>
    void iterate_directory(File& directory, Callback&& callback)
    {
        assert(directory.directory_size() <= directory_entries_per_block);

        DirectoryEntry *entries = reinterpret_cast<DirectoryEntry*>(directory.m_direct_blocks[0]);
        for (usize index = 0; index < directory.directory_size(); ++index) {
            if (callback(entries[index]) == IterationDecision::Break)
                return;
        }
    }

    class MemoryFilesystem : public Singleton<MemoryFilesystem> {
    public:
        File& root() { return *m_root; }

        // This won't work with multiple devices
        File& lookup_inode(usize inode)
        {
            return *m_inodes.lookup(inode).must();
        }

        File& create_directory(File& directory, StringView filename, u32 mode, u32 device)
        {
            auto& file = create_file(directory, filename, mode | S_IFDIR, device);

            file.append_file(".", file);
            file.append_file("..", directory);

            return file;
        }

        File& create_file(File& directory, StringView filename, u32 mode, u32 device)
        {
            assert(directory.is_directory());
            assert(directory.directory_size() < directory_entries_per_block);

            File& file = *new File;
            file.m_device_id = device;
            file.m_inode = m_next_inode++;
            file.m_mode = mode;
            file.m_size = 0;
            file.m_direct_blocks[0] = new u8[block_size];
            m_inodes.append(file.m_inode, &file);

            DirectoryEntry& entry = *reinterpret_cast<DirectoryEntry*>(directory.m_direct_blocks[0] + directory.m_size);
            entry.m_inode = file.m_inode;
            filename.strcpy_to({ entry.m_name, sizeof(entry.m_name) });
            directory.m_size += sizeof(DirectoryEntry);

            return file;
        }

    private:
        friend Singleton<MemoryFilesystem>;
        MemoryFilesystem()
        {
            DirectoryEntry *entries = reinterpret_cast<DirectoryEntry*>(new u8[block_size]);

            entries[0].m_inode = 2;
            __builtin_strcpy(entries[0].m_name, ".");

            entries[1].m_inode = 2;
            __builtin_strcpy(entries[1].m_name, "..");

            m_root = new File;
            m_root->m_device_id = device_ram;
            m_root->m_inode = 2;
            m_root->m_mode = S_IFDIR;
            m_root->m_size = sizeof(DirectoryEntry) * 2;
            m_root->m_direct_blocks[0] = reinterpret_cast<u8*>(entries);
            m_inodes.append(2, m_root);

            m_next_inode = 3;

            load_flash_filesystem();
        }

        void load_flash_filesystem()
        {
            for(auto *entry = __embed_start; entry < __embed_end; entry++)
            {
                dbgln("Loading % from flash", entry->m_path);

                File *directory = &root();
                iterate_split_path(entry->m_path, [&](StringView part, bool final) {
                    if (final) {
                        char *null_terminated_part = new char[part.size() + 1];
                        memcpy(null_terminated_part, part.data(), part.size());
                        null_terminated_part[part.size()] = 0;

                        dbgln("Creating file % in inode %", part, directory->m_inode);
                        directory->append_file(null_terminated_part, *(File*)entry->m_inode);

                        delete null_terminated_part;
                        return IterationDecision::Break;
                    }

                    bool exists = false;
                    iterate_directory(*directory, [&](DirectoryEntry entry) {
                        if (part == entry.m_name) {
                            directory = &lookup_inode(entry.m_inode);
                            exists = true;
                            return IterationDecision::Break;
                        }

                        return IterationDecision::Continue;
                    });

                    if (!exists) {
                        dbgln("Creating directory % in inode %", part, directory->m_inode);
                        directory = &create_directory(*directory, part, 0, device_flash);
                    }

                    return IterationDecision::Continue;
                });
            }
        }

        Map<u32, File*> m_inodes;

        File *m_root;
        u32 m_next_inode;
    };

    template<typename Callback = decltype([](File&, bool){ return IterationDecision::Continue; })>
    File& iterate_path(StringView path, Callback&& callback = {})
    {
        MemoryFilesystem& fs = MemoryFilesystem::the();

        File *file = &fs.root();
        iterate_split_path(path, [&](StringView part, bool final) {
            dbgln("Looking for % in inode %", part, file->m_inode);

            bool found_file = false;
            iterate_directory(*file, [&](DirectoryEntry entry) {
                if (entry.name() == part) {
                    found_file = true;
                    file = &fs.lookup_inode(entry.m_inode);
                    return IterationDecision::Break;
                }

                dbgln("Nope, it's not %", entry.name());
                return IterationDecision::Continue;
            });
            assert(found_file);

            if (callback(*file, final) == IterationDecision::Break)
                return IterationDecision::Break;

            return IterationDecision::Continue;
        });

        return *file;
    }
}
