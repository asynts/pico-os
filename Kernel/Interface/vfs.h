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

#ifdef HOST
# define FLASH_TYPE_DIR 1
# define FLASH_TYPE_REG 2
#else
# define S_IFMT 0xf
# define S_IFDIR 1
# define S_IFREG 2
#endif

struct FileInfo {
    uint32_t m_id;
    uint32_t m_device;
    uint32_t m_mode;
    uint32_t m_size;

#ifdef HOST
    uint32_t m_direct_blocks_raw[1];
#else
    uint8_t *m_direct_blocks[1];
#endif

#ifdef HOST
    uint32_t m_indirect_blocks_raw[4];
#else
    uint8_t **m_indirect_blocks[4];
#endif
};

struct FlashDirectoryEntryInfo {
    char m_name[252];

#ifdef HOST
    uint32_t m_info_raw;
#else
    FileInfo *m_info;
#endif
};

struct FlashLookupEntry {
    uint32_t m_id;

#ifdef HOST
    uint32_t m_info_raw;
#else
    FileInfo *m_info;
#endif
};
