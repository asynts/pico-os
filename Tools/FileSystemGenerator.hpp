#pragma once

#include <filesystem>
#include <cstddef>
#include <cassert>

#include <stdint.h>
#include <sys/stat.h>
#include <bsd/string.h>

#include "BufferStream.hpp"
#include "ElfGenerator.hpp"

// FIXME: Get this from <Kernel/Interface/inode.h>
struct IndexNode {
    uint32_t m_inode;
    uint32_t m_mode;
    uint32_t m_size;
    uint32_t m_device_id;
    uint8_t *m_direct_blocks[1];
    uint8_t **m_indirect_blocks[4];
};

struct RegistryEntry {
    char m_name[256];
    IndexNode *m_inode;
};

constexpr size_t FLASH_DEVICE_ID = 2;
constexpr size_t BLOCK_SIZE = 0x1000;
constexpr size_t REFERENCES_PER_BLOCK = BLOCK_SIZE / 4;

class FileSystemGenerator {
public:
    void add_file(std::filesystem::path path, uint32_t address, uint32_t size)
    {
        const uint32_t max_address = address + size;

        IndexNode inode;
        inode.m_inode = m_next_inode++;
        inode.m_mode = S_IFREG;
        inode.m_size = size;
        inode.m_device_id = FLASH_DEVICE_ID;

        inode.m_direct_blocks[0] = reinterpret_cast<uint8_t*>(address);
        assert(sizeof(inode.m_direct_blocks) / sizeof(*inode.m_direct_blocks));
        address += BLOCK_SIZE;

        size_t indirect_block_index = 0;
        while (address < max_address) {
            uint8_t *indirect_block[REFERENCES_PER_BLOCK];

            for (size_t index = 0; index < REFERENCES_PER_BLOCK; ++index) {
                if (address >= max_address) {
                    indirect_block[index] = nullptr;
                } else {
                    indirect_block[index] = reinterpret_cast<uint8_t*>(address);
                    address += BLOCK_SIZE;
                }
            }

            size_t indirect_block_address = m_metadata_stream.write_bytes({ (const uint8_t*)indirect_block, BLOCK_SIZE });
            inode.m_indirect_blocks[indirect_block_index++] = (uint8_t**)indirect_block_address;
        }

        RegistryEntry entry;
        strlcpy(entry.m_name, path.c_str(), sizeof(entry.m_name));
        entry.m_inode = (IndexNode*)m_metadata_stream.write_object(inode);

        m_index_stream.write_object((uint32_t)m_metadata_stream.write_object(entry));
    }

    void finalize(ElfGenerator& elf_generator) &&
    {
        elf_generator.append_section(".embedded.metadata", m_metadata_stream.map_into_memory());
        elf_generator.append_section(".embedded.index", m_index_stream.map_into_memory());
    }

private:
    size_t m_next_inode = 3;

    BufferStream m_metadata_stream;
    BufferStream m_index_stream;
};
