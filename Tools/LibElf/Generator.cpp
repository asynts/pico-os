#include <assert.h>
#include <string.h>

#include "Generator.hpp"

namespace Elf
{
    Generator::Generator()
        // : m_shstrtab(".shstrtab")
        // , m_symtab(*this, "")
    {
        // We write the elf header in ElfGenerator::finalize
        m_stream.seek(sizeof(Elf32_Ehdr));

        create_undefined_section();

        m_shstrtab.emplace(*this, ".shstrtab");
        m_symtab.emplace(*this, "");
    }
    void Generator::create_undefined_section()
    {
        Elf32_Shdr shdr;
        shdr.sh_addr = 0;
        shdr.sh_addralign = 0;
        shdr.sh_entsize = 0;
        shdr.sh_flags = 0;
        shdr.sh_info = 0;
        shdr.sh_link = 0;
        shdr.sh_name = 0;
        shdr.sh_offset = 0;
        shdr.sh_size = 0;
        shdr.sh_type = SHT_NULL;
        m_sections.push_back(shdr);
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
        shdr.sh_name = m_shstrtab->add_entry(name);
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

        section.sh_addr = m_stream.offset() - sizeof(Elf32_Ehdr);
        section.sh_offset = m_stream.offset();
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
    void Generator::encode_sections(size_t& section_offset, size_t& shstrtab_section_index)
    {
        m_symtab->apply();
        m_shstrtab->apply();

        shstrtab_section_index = m_shstrtab->strtab_index();

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
