#pragma once

#include <cassert>
#include <vector>
#include <cstring>
#include <fmt/format.h>

#include <elf.h>

#include "BufferStream.hpp"

class StringTable {
public:
    explicit StringTable(std::string_view name_suffix)
        : m_name_suffix(name_suffix)
    {
        add_entry("");
    }

    size_t add_entry(std::string_view name)
    {
        size_t offset = m_strtab_stream.write_bytes({ (const uint8_t*)name.data(), name.size() });
        m_strtab_stream.write_object<uint8_t>(0);
        return offset;
    }

    void apply(ElfGenerator& generator)
    {
        assert(!m_applied);
        m_applied = true;

        // We do this in two steps, because this might be the .shstrtab string table
        m_strtab_index = generator.create_section(fmt::format(".strtab.{}", m_name_suffix), SHT_STRTAB, 0);
        generator.write_section(m_strtab_index, m_strtab_stream);
    }

    size_t strtab_index()
    {
        return m_strtab_index.value();
    }

private:
    BufferStream m_strtab_stream;

    std::string_view m_name_suffix;
    bool m_applied = false;
    std::optional<size_t> m_strtab_index;
};

class SymbolTable {
public:
    explicit SymbolTable(std::string_view name_suffix, size_t target_index)
        : m_name_suffix(name_suffix)
        , m_string_table(name_suffix)
        , m_target_index(target_index)
    {
    }

    size_t add_symbol(std::string_view name, Elf32_Sym symbol)
    {
        symbol.st_name = m_string_table.add_entry(name);
        m_symtab_stream.write_object(symbol);
        return m_next_index++;
    }

    void add_relocation(Elf32_Rel relocation)
    {
        m_rel_stream.write_object(relocation);
    }

    void add_relocation(Elf32_Rela relocation)
    {
        m_rel_stream.write_object(relocation);
    }

    void apply(ElfGenerator& generator)
    {
        assert(!m_applied);
        m_applied = true;

        m_string_table.apply(generator);

        m_symtab_index = generator.append_section(fmt::format(".symtab.{}", m_name_suffix), m_symtab_stream, SHT_SYMTAB, 0);
        auto& symtab_section = generator.section(m_symtab_index.value());

        symtab_section.sh_entsize = sizeof(Elf32_Sym);
        symtab_section.sh_link = m_string_table.strtab_index();
        symtab_section.sh_info = m_next_index;

        m_rel_index = generator.append_section(fmt::format(".rel.{}", m_name_suffix), m_rel_stream, SHT_REL, 0);
        auto& rel_section = generator.section(m_rel_index.value());

        rel_section.sh_entsize = sizeof(Elf32_Rel);
        rel_section.sh_link = m_symtab_index;
        rel_section.sh_info = m_target_index;

        m_rela_index = generator.append_section(fmt::format(".rela.{}", m_name_suffix), m_rela_stream, SHT_RELA, 0);
        auto& rela_section = generator.section(m_rela_index.value());

        rela_section.sh_entsize = sizeof(Elf32_Rela);
        rela_section.sh_link = m_symtab_index;
        rela_section.sh_info = m_target_index;
    }

private:
    StringTable m_string_table;
    BufferStream m_symtab_stream;
    BufferStream m_rel_stream;
    BufferStream m_rela_stream;

    std::string_view m_name_suffix;
    size_t m_next_index = 0;
    bool m_applied = false;

    size_t m_target_index;
    std::optional<size_t> m_symtab_index;
    std::optional<size_t> m_rel_index;
    std::optional<size_t> m_rela_index;
};

class ElfGenerator {
public:
    ElfGenerator()
    {
        // We write the elf header in ElfGenerator::finalize
        m_stream.seek(sizeof(Elf32_Ehdr));

        create_section("", SHT_NULL, 0);
    }

    Elf32_Shdr& section(size_t index) { return m_sections[index]; }

    size_t append_section(
        std::string_view name,
        BufferStream& stream,
        Elf32_Word type = SHT_PROGBITS,
        Elf32_Word flags = SHF_ALLOC)
    {
        size_t index = create_section(name, type, flags);
        write_section(index, stream);
        return index;
    }

    size_t create_section(
        std::string_view name,
        Elf32_Word type = SHT_PROGBITS,
        Elf32_Word flags = SHF_ALLOC)
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

    void write_section(size_t section_index, BufferStream& stream)
    {
        auto& section = this->section(section_index);

        section.sh_addr = stream.offset() - sizeof(Elf32_Ehdr);
        section.sh_offset = stream.offset();
        section.sh_size = stream.size();

        m_stream.write_bytes(stream);
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
        size_t index = create_section(".shstrtab", SHT_STRTAB, 0);
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
