#include <map>

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
struct DirectoryEntry {
    char m_name[252];
    uint32_t m_inode;
};
struct LookupEntry {
    uint32_t m_inode_number;
    uint32_t m_inode_pointer;
};

FileSystem::FileSystem(Elf::Generator& generator)
    : m_generator(generator)
{
    m_data_index = m_generator.create_section(".embed", SHT_PROGBITS, SHF_ALLOC);
    m_data_relocs.emplace(m_generator, ".embed", generator.symtab().symtab_index(), *m_data_index);
}
FileSystem::~FileSystem()
{
    assert(m_finalized);
}
uint32_t FileSystem::add_host_file(std::string_view path)
{
    // FIXME: We don't have to map the file into memory
    Elf::MemoryStream stream;
    stream.write_bytes(Elf::mmap_file(path));

    return add_file(stream);
}
uint32_t FileSystem::add_directory(std::map<std::string, uint32_t>& files, uint32_t inode_number)
{
    Elf::MemoryStream stream;

    for (auto& [name, inode] : files) {
        DirectoryEntry entry;
        strlcpy(entry.m_name, name.c_str(), sizeof(entry.m_name));
        entry.m_inode = inode;
    }

    return add_file(stream, S_IFDIR, inode_number);
}
uint32_t FileSystem::add_root_directory(std::map<std::string, uint32_t>& files)
{
    return add_directory(files, 2);
}
uint32_t FileSystem::add_file(Elf::MemoryStream& stream, uint32_t mode, uint32_t inode_number)
{
    if (inode_number == 0)
        inode_number = m_next_inode++;

    size_t data_offset = m_data_stream.write_bytes(stream);
    size_t max_data_offset = data_offset + stream.size();
    size_t data_symbol = m_generator.symtab().add_symbol(fmt::format("__flash_data_{}", inode_number), Elf32_Sym {
        .st_value = (uint32_t)data_offset,
        .st_size = (uint32_t)stream.size(),
        .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
        .st_other = STV_DEFAULT,
        .st_shndx = static_cast<uint16_t>(*m_data_index),
    });

    IndexNode inode;
    inode.m_inode = inode_number;
    inode.m_mode = mode;
    inode.m_size = stream.size();
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
        size_t blocks_symbol = m_generator.symtab().add_symbol(fmt::format("__flash_iblock_{}_{}", inode_number, indirect_block_index), Elf32_Sym {
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

        inode.m_indirect_blocks[indirect_block_index] = 0;
        inode_relocations.push_back(Elf32_Rel {
            .r_offset = (uint32_t) (offsetof(IndexNode, m_indirect_blocks) + indirect_block_index * sizeof(uint32_t)),
            .r_info = ELF32_R_INFO(blocks_symbol, R_ARM_ABS32),
        });

        ++indirect_block_index;
    }

    size_t inode_offset = m_data_stream.write_object(inode);
    size_t inode_symbol = m_generator.symtab().add_symbol(fmt::format("__flash_header_{}", inode_number), Elf32_Sym {
        .st_value = static_cast<uint32_t>(inode_offset),
        .st_size = sizeof(IndexNode),
        .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
        .st_other = STV_DEFAULT,
        .st_shndx = static_cast<uint16_t>(*m_data_index),
    });

    if (inode_number == 2) {
        m_generator.symtab().add_symbol("__flash_root", Elf32_Sym {
            .st_value = static_cast<uint32_t>(inode_offset),
            .st_size = sizeof(IndexNode),
            .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
            .st_other = STV_DEFAULT,
            .st_shndx = static_cast<uint16_t>(*m_data_index),
        });
    }

    for (Elf32_Rel& relocation : inode_relocations)
    {
        relocation.r_offset += inode_offset;
        m_data_relocs->add_entry(relocation);
    }

    m_inode_to_offset[inode_number] = inode_offset;

    return inode.m_inode;
}
void FileSystem::finalize()
{
    assert(!m_finalized);
    m_finalized = true;

    m_data_relocs->finalize();
    m_generator.write_section(m_data_index.value(), m_data_stream);

    size_t data_symbol = m_generator.symtab().add_symbol("__flash_base", Elf32_Sym {
        .st_value = 0,
        .st_size = static_cast<uint32_t>(m_data_stream.size()),
        .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
        .st_other = STV_DEFAULT,
        .st_shndx = static_cast<uint16_t>(*m_data_index),
    });

    size_t tab_index = m_generator.create_section(".embed.tab", SHT_PROGBITS, SHF_ALLOC);
    Elf::RelocationTable tab_relocs { m_generator, ".embed.tab", m_generator.symtab().symtab_index(), tab_index };

    // Note that std::map iterators are sorted by the key
    Elf::MemoryStream tab_stream;
    for (auto& [inode_number, inode_offset] : m_inode_to_offset) {
        size_t lookup_offset = tab_stream.write_object(LookupEntry { inode_number, inode_offset });

        tab_relocs.add_entry(Elf32_Rel {
            .r_offset = static_cast<uint32_t>(lookup_offset + offsetof(LookupEntry, m_inode_pointer)),
            .r_info = ELF32_R_INFO(data_symbol, R_ARM_ABS32),
        });
    }

    tab_relocs.finalize();
    m_generator.write_section(tab_index, tab_stream);

    m_generator.symtab().add_symbol("__flash_tab_start", Elf32_Sym {
        .st_value = 0,
        .st_size = 0,
        .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
        .st_other = STV_DEFAULT,
        .st_shndx = static_cast<uint16_t>(tab_index),
    });
    m_generator.symtab().add_symbol("__flash_tab_end", Elf32_Sym {
        .st_value = static_cast<uint32_t>(tab_stream.size()),
        .st_size = 0,
        .st_info = ELF32_ST_INFO(STB_GLOBAL, STT_OBJECT),
        .st_other = STV_DEFAULT,
        .st_shndx = static_cast<uint16_t>(tab_index),
    });
}
