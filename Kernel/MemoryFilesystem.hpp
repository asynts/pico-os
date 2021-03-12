#pragma once

#include <Std/Singleton.hpp>
#include <Std/StringView.hpp>

#define S_IFMT  0b11
#define S_IFCHR 0b01
#define S_IFDIR 0b10
#define S_IFREG 0b11

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

    struct File {
        u32 m_inode;
        u32 m_mode;
        u32 m_size;
        u32 m_device_id;
        u8 *m_direct_blocks[1];

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
    };

    class MemoryFilesystem : public Singleton<MemoryFilesystem> {
    public:
        File& root() { return *m_root; }

        File& inode_to_pointer(usize inode)
        {
            // FIXME
            assert(inode == 2);
            return root();
        }

        File& create_file(File& directory, StringView filename, u32 mode)
        {
            assert(directory.is_directory());
            assert(directory.directory_size() < directory_entries_per_block);

            File& file = *new File;
            file.m_device_id = 0;
            file.m_inode = m_next_inode++;
            file.m_mode = mode;
            file.m_size = 0;
            file.m_direct_blocks[0] = new u8[block_size];

            DirectoryEntry& entry = *reinterpret_cast<DirectoryEntry*>(directory.m_direct_blocks[0] + directory.directory_size() * sizeof(DirectoryEntry));
            entry.m_inode = file.m_inode;
            filename.copy_to({ entry.m_name, sizeof(entry.m_name) });
            directory.m_size += sizeof(DirectoryEntry);

            return file;
        }

    private:
        friend Singleton<MemoryFilesystem>;
        MemoryFilesystem()
        {
            static_assert(0x200 % sizeof(DirectoryEntry) == 0);
            DirectoryEntry *entries = reinterpret_cast<DirectoryEntry*>(new u8[0x200]);

            entries[0].m_inode = 2;
            __builtin_strcpy(entries[0].m_name, ".");

            entries[0].m_inode = 2;
            __builtin_strcpy(entries[0].m_name, "..");

            m_root = new File;
            m_root->m_inode = 2;
            m_root->m_mode = S_IFDIR;
            m_root->m_size = sizeof(DirectoryEntry) * 2;
            m_root->m_device_id = 0;
            m_root->m_direct_blocks[0] = reinterpret_cast<u8*>(entries);

            m_next_inode = 3;

            auto& bin_dir = create_file(root(), "bin", S_IFDIR);

            // FIXME: Hook up file with flash. m_device should indicate FLASH.
            auto& shell_file = create_file(bin_dir, "Shell.elf", S_IFREG);
        }

        File *m_root;
        u32 m_next_inode;
    };

    template<typename Callback>
    void iterate_directory(File& directory, Callback&& callback = []{})
    {
        assert(directory.is_directory());

        for (usize index = 0; index < directory.directory_size(); ++index)
        {
            DirectoryEntry *entry = directory.m_direct_blocks[0] + index * sizeof(DirectoryEntry);
            callback(*entry);
        }
    }

    template<typename Callback>
    File& iterate_path(StringView path, File& root, Callback&& callback = []{})
    {
        callback(root);

        if (path.size() == 0)
            return root;

        assert(root.is_directory());

        assert(path[0] == '/');
        path = path.substr(1);

        auto end = path.index_of('/').value_or(path.size());
        StringView filename = path.trim(end);

        File *file = nullptr;
        iterate_directory(root, [&](DirectoryEntry& entry){
            if (entry.name() == filename)
                file = MemoryFilesystem::the().inode_to_pointer(entry.m_inode);
        });
        assert(file != nullptr);

        return iterate_path(path.substr(end, *file, move(callback)));
    }
}
