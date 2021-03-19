#pragma once

#include <stdint.h>

// In this filesystem `m_direct_blocks[0]` is null and `DirectoryEntryInfo`s are generated
// with `m_keep` set.
#define RAM_DEVICE_ID 1

// In this filesystem `m_direct_blocks[0]` is a pointer to the data which is stored
// contiguously.
//
// Directories are encoded as normal files where the data is an array of
// `FlashDirectoryEntryInfo`s which can be used to generate `DirectoryEntryInfo`s.
#define FLASH_DEVICE_ID 2
#define FLASH_BLOCK_SIZE 0x1000
#define FLASH_BLOCK_ENTRIES 0x400

#define S_IFDIR 1
#define S_IFREG 2

struct FileInfo {
    uint32_t m_id;
    uint32_t m_device;
    uint32_t m_mode;
    uint32_t m_size;

    union {
        uint8_t *m_direct_blocks[1];
        uint32_t m_direct_blocks_raw[1];
    };

    union {
        uint8_t **m_indirect_blocks[4];
        uint32_t m_indirect_blocks_raw[4];
    };
};

struct FlashDirectoryEntryInfo {
    char m_name[252];

    union {
        FileInfo *m_info;
        uint32_t m_info_raw;
    };
};

struct FlashLookupEntry {
    uint32_t m_id;

    union {
        FileInfo *m_info;
        uint32_t m_info_raw;
    };
};
