#include <assert.h>

#include <fmt/format.h>

#include "SymbolTable.hpp"
#include "Generator.hpp"

// FIXME: I think I messed up the target index thing

namespace Elf
{
    SymbolTable::SymbolTable(std::string_view name_suffix, size_t target_index)
        : m_name_suffix(name_suffix)
        , m_string_table(fmt::format(".strtab.{}", name_suffix))
        , m_target_index(target_index)
    {
        Elf32_Sym undefined_symbol;
        undefined_symbol.st_info = 0;
        undefined_symbol.st_other = 0;
        undefined_symbol.st_shndx = SHN_UNDEF;
        undefined_symbol.st_size = 0;
        undefined_symbol.st_value = 0;
        add_symbol("", undefined_symbol);
    }
    size_t SymbolTable::add_symbol(std::string_view name, Elf32_Sym symbol)
    {
        symbol.st_shndx = m_target_index;
        symbol.st_name = m_string_table.add_entry(name);
        m_symtab_stream.write_object(symbol);
        return m_next_index++;
    }
    void SymbolTable::add_relocation(Elf32_Rel relocation)
    {
        m_rel_stream.write_object(relocation);
    }
    void SymbolTable::add_relocation(Elf32_Rela relocation)
    {
        m_rel_stream.write_object(relocation);
    }
    void SymbolTable::apply(Generator& generator)
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
        rel_section.sh_link = m_symtab_index.value();
        rel_section.sh_info = m_target_index;

        m_rela_index = generator.append_section(fmt::format(".rela.{}", m_name_suffix), m_rela_stream, SHT_RELA, 0);
        auto& rela_section = generator.section(m_rela_index.value());

        rela_section.sh_entsize = sizeof(Elf32_Rela);
        rela_section.sh_link = m_symtab_index.value();
        rela_section.sh_info = m_target_index;
    }   
}
