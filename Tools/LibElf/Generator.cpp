#include <assert.h>
#include <string.h>

#include "Generator.hpp"

namespace Elf
{
    Generator::Generator()
    {
        // We write the elf header in ElfGenerator::finalize
        m_stream.seek(sizeof(Elf32_Ehdr));

        create_section("", SHT_NULL, 0);
    }
    size_t Generator::append_section(std::string_view name, MemoryStream& stream, Elf32_Word type, Elf32_Word flags)
    {
        size_t index = create_section(name, type, flags);
        write_section(index, stream);
        return index;
    }
    size_t Generator::create_section(std::string_view name, Elf32_Word type, Elf32_Word flags)
    {
        Elf32_Shdr shdr;
        shdr.sh_addr = 0;
        shdr.sh_addralign = 4;
        shdr.sh_entsize = 0;
        shdr.sh_flags = flags;
        shdr.sh_info = 0;
        shdr.sh_link = 0;
        shdr.sh_name = append_section_name(name);
        shdr.sh_offset = 0;
        shdr.sh_size = 0;
        shdr.sh_type = type;
        
        size_t index = m_sections.size();
        m_sections.push_back(shdr);
        return index;
    }
    void Generator::write_section(size_t section_index, MemoryStream& stream)
    {
        auto& section = this->section(section_index);

        section.sh_addr = stream.offset() - sizeof(Elf32_Ehdr);
        section.sh_offset = stream.offset();
        section.sh_size = stream.size();

        m_stream.write_bytes(stream);
    }
    MemoryStream Generator::finalize() &&
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
    size_t Generator::append_section_name(std::string_view name)
    {
        size_t offset = m_shstrtab_stream.offset();
        m_shstrtab_stream.write_bytes({ (const uint8_t*)name.data() , name.size() });

        uint8_t null_terminator = 0;
        static_assert(sizeof(null_terminator) == 1);
        m_shstrtab_stream.write_object(null_terminator);

        return offset;
    }
    size_t Generator::append_shstrtab_section()
    {
        size_t index = create_section(".shstrtab", SHT_STRTAB, 0);
        auto& section = m_sections[index];

        size_t offset = m_stream.write_bytes(m_shstrtab_stream);

        section.sh_addr = offset - sizeof(Elf32_Ehdr);
        section.sh_offset = offset;
        section.sh_size = m_shstrtab_stream.size();

        return index;
    }
    void Generator::encode_sections(size_t& section_offset, size_t& shstrtab_section_index)
    {
        shstrtab_section_index = append_shstrtab_section();

        section_offset = m_stream.offset();

        for (const Elf32_Shdr& section : m_sections)
            m_stream.write_object(section);
    }
    void Generator::encode_header(size_t section_offset, size_t shstrtab_section_index)
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
}
