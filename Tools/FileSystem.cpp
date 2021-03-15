#include <bsd/string.h>
#include <sys/stat.h>

#include <fmt/format.h>

#include <LibElf/MemoryStream.hpp>

#include "FileSystem.hpp"

// FIXME: Get this from some kernel header
#define FLASH_DEVICE_ID 1
#define FLASH_BLOCK_SIZE 0x1000
#define FLASH_BLOCK_ENTRIES 0x400
struct IndexNode {
    uint32_t m_inode;
    uint32_t m_mode;
    uint32_t m_size;
    uint32_t m_device_id;
    uint32_t m_block_size;
    uint32_t m_direct_blocks[1];
    uint32_t m_indirect_blocks[4];
};

FileSystem::FileSystem(Elf::Generator& generator)
    : m_generator(generator)
{
    m_data_index = generator.create_section(".embed", SHT_PROGBITS, SHF_ALLOC);
    m_data_symtab.emplace(".embed", m_data_index.value());
}
void FileSystem::add_file(std::string_view path, std::span<const uint8_t> data)
{
    // FIXME: Create relocations for everything

    size_t data_offset = m_data_stream.write_bytes(data);
    size_t max_data_offset = data_offset + data.size();

    IndexNode inode;
    inode.m_inode = m_next_inode++;
    inode.m_mode = S_IFREG;
    inode.m_size = data.size();
    inode.m_device_id = FLASH_DEVICE_ID;
    inode.m_block_size = FLASH_BLOCK_SIZE;

    inode.m_direct_blocks[0] = data_offset;
    data_offset += FLASH_BLOCK_SIZE;

    size_t indirect_block_index = 0;
    while (data_offset < max_data_offset)
    {
        uint32_t blocks[FLASH_BLOCK_ENTRIES];

        for (size_t index = 0; index < FLASH_BLOCK_ENTRIES; ++index)
        {
            if (data_offset >= max_data_offset)
                break;
            
            blocks[index] = data_offset;
            data_offset += FLASH_BLOCK_SIZE;
        }

        inode.m_indirect_blocks[indirect_block_index] = m_data_stream.write_bytes({ (const uint8_t*)blocks, sizeof(blocks) });
    }

    size_t inode_offset = m_data_stream.write_object(inode);

    m_data_symtab->add_symbol(fmt::format("flash-fs:{}", path), Elf32_Sym {
        .st_value = static_cast<uint32_t>(inode_offset),
        .st_size = sizeof(IndexNode),
        .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_NOTYPE),
        .st_other = STV_DEFAULT,
    });
}
void FileSystem::finalize() &&
{
    m_data_symtab->apply(m_generator);

    m_generator.write_section(m_data_index.value(), m_data_stream);
}
