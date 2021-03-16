#include <sys/stat.h>
#include <bsd/string.h>
#include <assert.h>

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

struct FlashEntry {
    char m_name[252];
    uint32_t m_inode;
};
static_assert(sizeof(FlashEntry) == 256);

FileSystem::FileSystem(Elf::Generator& generator)
    : m_generator(generator)
{
    m_data_index = m_generator.create_section(".embed", SHT_PROGBITS, SHF_ALLOC);
    m_data_relocs.emplace(m_generator, ".embed", generator.symtab().symtab_index(), *m_data_index);

    m_tab_index = m_generator.create_section(".embed.tab", SHT_PROGBITS, SHF_ALLOC);
    m_tab_relocs.emplace(m_generator, ".embed.tab", generator.symtab().symtab_index(), *m_tab_index);
}
FileSystem::~FileSystem()
{
    assert(m_finalized);
}
void FileSystem::add_file(std::string_view path, std::span<const uint8_t> data)
{
    size_t data_offset = m_data_stream.write_bytes(data);
    size_t max_data_offset = data_offset + data.size();
    size_t data_symbol = m_generator.symtab().add_symbol(fmt::format("fs:data:{}", path), Elf32_Sym {
        .st_value = (uint32_t)data_offset,
        .st_size = (uint32_t)data.size(),
        .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
        .st_other = STV_DEFAULT,
        .st_shndx = static_cast<uint16_t>(*m_data_index),
    });

    IndexNode inode;
    inode.m_inode = m_next_inode++;
    inode.m_mode = S_IFREG;
    inode.m_size = data.size();
    inode.m_device_id = FLASH_DEVICE_ID;
    inode.m_block_size = FLASH_BLOCK_SIZE;

    std::vector<Elf32_Rel> inode_relocations;

    inode.m_direct_blocks[0] = data_offset;
    data_offset += FLASH_BLOCK_SIZE;
    inode_relocations.push_back(Elf32_Rel {
        .r_offset = offsetof(IndexNode, m_direct_blocks),
        .r_info = ELF32_R_INFO(data_symbol, R_ARM_ABS32),
    });

    size_t indirect_block_index = 0;
    while (data_offset < max_data_offset)
    {
        uint32_t blocks[FLASH_BLOCK_ENTRIES];

        std::vector<Elf32_Rel> blocks_relocations;

        for (size_t index = 0; index < FLASH_BLOCK_ENTRIES; ++index)
        {
            if (data_offset >= max_data_offset)
                break;
            
            blocks[index] = data_offset;
            data_offset += FLASH_BLOCK_SIZE;
            blocks_relocations.push_back(Elf32_Rel {
                .r_offset = (uint32_t) (index * sizeof(uint32_t)),
                .r_info = ELF32_R_INFO(data_symbol, R_ARM_ABS32),
            });
        }

        size_t blocks_offset = m_data_stream.write_bytes({ (const uint8_t*)blocks, sizeof(blocks) });
        size_t blocks_symbol = m_generator.symtab().add_symbol(fmt::format("fs:iblock:{}:{}", indirect_block_index, path), Elf32_Sym {
            .st_name = 0,
            .st_value = (uint32_t)blocks_offset,
            .st_size = sizeof(blocks),
            .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
            .st_other = STV_DEFAULT,
            .st_shndx = static_cast<uint16_t>(*m_data_index),
        });

        for (Elf32_Rel& relocation : blocks_relocations) {
            relocation.r_offset += blocks_offset;
            m_data_relocs->add_entry(relocation);
        }

        inode.m_indirect_blocks[indirect_block_index] = blocks_offset;
        inode_relocations.push_back(Elf32_Rel {
            .r_offset = (uint32_t) (offsetof(IndexNode, m_indirect_blocks) + indirect_block_index * sizeof(uint32_t)),
            .r_info = ELF32_R_INFO(blocks_symbol, R_ARM_ABS32),
        });

        ++indirect_block_index;
    }

    size_t inode_offset = m_data_stream.write_object(inode);
    size_t inode_symbol = m_generator.symtab().add_symbol(fmt::format("fs:inode:{}", path), Elf32_Sym {
        .st_value = static_cast<uint32_t>(inode_offset),
        .st_size = sizeof(IndexNode),
        .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
        .st_other = STV_DEFAULT,
        .st_shndx = static_cast<uint16_t>(*m_data_index),
    });

    for (Elf32_Rel& relocation : inode_relocations)
    {
        relocation.r_offset += inode_offset;
        m_data_relocs->add_entry(relocation);
    }

    FlashEntry entry;
    entry.m_inode = 0;
    strlcpy(entry.m_name, std::string { path }.c_str(), sizeof(entry.m_name));
    size_t entry_offset = m_tab_stream.write_object(entry);

    m_generator.symtab().add_symbol(fmt::format("fs:entry:{}", path), Elf32_Sym {
        .st_value = (uint32_t)entry_offset,
        .st_size = sizeof(FlashEntry),
        .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
        .st_other = STV_DEFAULT,
        .st_shndx = static_cast<uint16_t>(*m_tab_index),
    });
    m_tab_relocs->add_entry(Elf32_Rel {
        .r_offset = (uint32_t) (entry_offset + offsetof(FlashEntry, m_inode)),
        .r_info = ELF32_R_INFO(data_symbol, R_ARM_ABS32),
    });
}
void FileSystem::finalize()
{
    assert(!m_finalized);
    m_finalized = true;

    m_data_relocs->finalize();
    m_tab_relocs->finalize();

    m_generator.write_section(m_data_index.value(), m_data_stream);
    m_generator.write_section(m_tab_index.value(), m_tab_stream);
}
