#pragma once

#include <cassert>
#include <vector>
#include <cstring>

#include <elf.h>

#include "BufferStream.hpp"

class ElfGenerator {
public:
    ElfGenerator()
    {
        // We write the elf header in ElfGenerator::finalize
        m_stream.seek(sizeof(Elf32_Ehdr));

        create_section("", 0, 0, 0, SHT_NULL, 0);
    }

    Elf32_Shdr& section(size_t index) { return m_sections[index]; }

    size_t append_section(
        std::string_view name,
        BufferStream& stream,
        Elf32_Word type = SHT_PROGBITS,
        Elf32_Word flags = SHF_ALLOC)
    {
        Elf32_Addr address = m_stream.offset() - sizeof(Elf32_Ehdr);
        Elf32_Off offset = m_stream.offset();
        Elf32_Word size = stream.size();

        m_stream.write_bytes(stream);

        return create_section(name, address, offset, size, type, flags);
    }

    size_t create_section(
        std::string_view name,
        Elf32_Addr address,
        Elf32_Off offset,
        Elf32_Word size,
        Elf32_Word type = SHT_PROGBITS,
        Elf32_Word flags = SHF_ALLOC)
    {
        Elf32_Shdr shdr;
        shdr.sh_addr = address;
        shdr.sh_addralign = 4;
        shdr.sh_entsize = 0;
        shdr.sh_flags = flags;
        shdr.sh_info = 0;
        shdr.sh_link = 0;
        shdr.sh_name = append_section_name(name);
        shdr.sh_offset = offset;
        shdr.sh_size = size;
        shdr.sh_type = type;
        
        size_t index = m_sections.size();
        m_sections.push_back(shdr);
        return index;
    }

    BufferStream finalize() &&
    {
        assert(!m_finalized);
        m_finalized = true;

        size_t section_offset;
        size_t shstrtab_section_index;
        encode_sections(section_offset, shstrtab_section_index);

        encode_header(section_offset, shstrtab_section_index);

        m_stream.seek(0);
        return std::move(m_stream);
    }

private:
    size_t append_section_name(std::string_view name)
    {
        size_t offset = m_shstrtab_stream.offset();
        m_shstrtab_stream.write_bytes({ (const uint8_t*)name.data() , name.size() });

        uint8_t null_terminator = 0;
        static_assert(sizeof(null_terminator) == 1);
        m_shstrtab_stream.write_object(null_terminator);

        return offset;
    }

    size_t append_shstrtab_section()
    {
        size_t index = create_section(".shstrtab", 0, 0, 0, SHT_STRTAB, 0);
        auto& section = m_sections[index];

        size_t offset = m_stream.write_bytes(m_shstrtab_stream);

        section.sh_addr = offset - sizeof(Elf32_Ehdr);
        section.sh_offset = offset;
        section.sh_size = m_shstrtab_stream.size();

        return index;
    }

    void encode_sections(size_t& section_offset, size_t& shstrtab_section_index)
    {
        shstrtab_section_index = append_shstrtab_section();

        section_offset = m_stream.offset();

        for (const Elf32_Shdr& section : m_sections)
            m_stream.write_object(section);
    }

    void encode_header(size_t section_offset, size_t shstrtab_section_index)
    {
        Elf32_Ehdr ehdr;
        ehdr.e_ehsize = sizeof(Elf32_Ehdr);
        ehdr.e_entry = 0;
        ehdr.e_flags = 0x05000000;
        
        memcpy(ehdr.e_ident, ELFMAG, SELFMAG);
        ehdr.e_ident[EI_CLASS] = ELFCLASS32;
        ehdr.e_ident[EI_DATA] = ELFDATA2LSB;
        ehdr.e_ident[EI_VERSION] = EV_CURRENT;
        ehdr.e_ident[EI_OSABI] = ELFOSABI_NONE;
        ehdr.e_ident[EI_ABIVERSION] = 0;

        ehdr.e_machine = EM_ARM;
        ehdr.e_phentsize = sizeof(Elf32_Phdr);
        ehdr.e_phnum = 0;
        ehdr.e_phoff = 0;
        ehdr.e_shentsize = sizeof(Elf32_Shdr);
        ehdr.e_shnum = m_sections.size();
        ehdr.e_shoff = section_offset;
        ehdr.e_shstrndx = shstrtab_section_index;
        ehdr.e_type = ET_REL;
        ehdr.e_version = EV_CURRENT;

        m_stream.seek(0);
        m_stream.write_object(ehdr);
    }

    BufferStream m_stream;

    bool m_finalized = false;
    std::vector<Elf32_Shdr> m_sections;
    BufferStream m_shstrtab_stream;
};
