#pragma once

#include <Std/Singleton.hpp>

#define S_IFMT  0b11
#define S_IFCHR 0b01
#define S_IFDIR 0b10

namespace Kernel {
    struct File {
        u32 m_inode;
        u32 m_mode;
        u32 m_size;
        u32 m_device_id;
        void *m_direct_blocks[1];
    };

    struct DirectoryEntry {
        u32 m_inode;
        char m_name[256 - sizeof(m_inode)];
    };

    class MemoryFilesystem : public Singleton<MemoryFilesystem> {
    public:
        File* root() { return m_root; }

    private:
        friend Singleton<MemoryFilesystem>;
        MemoryFilesystem()
        {
            auto *entries = new DirectoryEntry[2];

            entries[0].m_inode = 2;
            __builtin_strcpy(entries[0].m_name, ".");

            entries[0].m_inode = 2;
            __builtin_strcpy(entries[0].m_name, "..");

            m_root = new File;
            m_root->m_inode = 2;
            m_root->m_mode = S_IFDIR;
            m_root->m_size = sizeof(DirectoryEntry) * 2;
            m_root->m_device_id = 0;
            m_root->m_direct_blocks[0] = entries;
        }

        File *m_root;
    };
}
