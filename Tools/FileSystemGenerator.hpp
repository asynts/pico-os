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
    explicit FileSystemGenerator(ElfGenerator& elf_generator)
        : m_elf_generator(elf_generator)
    {
    }

    void create_file(std::string_view path, std::span<const uint8_t> data)
    {
        std::cout << "FileSystemGenerator::create_file path='" << path << "' size=" << data.size() << '\n';

        auto data_offset = m_data_stream.write_bytes(data);
        create_file_metadata(path, data_offset, data.size());
    }

    void create_file_metadata(std::string_view path, uint32_t data_offset, uint32_t size)
    {
        // FIXME: FileSystemGenerator::add_file from def84a4160dd1374f6aa475c26590bc164aa75d5.
    }

    void finalize() &&
    {
        m_elf_generator.append_section(".embed.data", m_data_stream);
        m_elf_generator.append_section(".embed.meta", m_meta_stream);
        m_elf_generator.append_section(".embed.tab", m_tab_stream);
    }

private:
    size_t m_next_inode = 3;
    ElfGenerator& m_elf_generator;

    BufferStream m_data_stream;
    BufferStream m_meta_stream;
    BufferStream m_tab_stream;
};
