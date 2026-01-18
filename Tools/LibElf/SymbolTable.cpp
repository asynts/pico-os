#include <assert.h>

#include <fmt/format.h>

#include "SymbolTable.hpp"
#include "Generator.hpp"

namespace Elf
{
    SymbolTable::SymbolTable(std::string_view name_suffix)
        : m_name(fmt::format(".symtab{}", name_suffix))
        , m_string_table(fmt::format(".strtab{}", name_suffix))
    {
        create_undefined_symbol();
    }
    SymbolTable::~SymbolTable()
    {
        assert(m_finalized);
    }
    void SymbolTable::create_undefined_symbol()
    {
        Elf32_Sym undefined_symbol;
        undefined_symbol.st_info = 0;
        undefined_symbol.st_other = 0;
        undefined_symbol.st_shndx = SHN_UNDEF;
        undefined_symbol.st_size = 0;
        undefined_symbol.st_value = 0;
        undefined_symbol.st_name = 0;
        m_stream.write_object(undefined_symbol);

        ++m_next_index;
    }
    size_t SymbolTable::add_symbol(std::string_view name, Elf32_Sym symbol)
    {
        // We don't support local symbols.
        assert(symbol.st_info & ELF32_ST_BIND(0xf) == ELF32_ST_BIND(STB_GLOBAL));

        symbol.st_name = m_string_table.add_entry(name);
        m_stream.write_object(symbol);
        return m_next_index++;
    }
    size_t SymbolTable::add_undefined_symbol(std::string_view name, Elf32_Sym symbol)
    {
        // We don't support local symbols.
        assert(symbol.st_info & ELF32_ST_BIND(0xf) == ELF32_ST_BIND(STB_GLOBAL));

        symbol.st_shndx = 0;
        symbol.st_name = m_string_table.add_entry(name);
        m_stream.write_object(symbol);
        return m_next_index++;
    }
    void SymbolTable::initialize(Generator& generator)
    {
        m_string_table.initialize(generator);

        m_section_index = generator.create_section(m_name, SHT_SYMTAB, 0);

        auto& symtab_section = generator.section(*m_section_index);
        symtab_section.sh_entsize = sizeof(Elf32_Sym);
        symtab_section.sh_link = m_string_table.section_index();
        symtab_section.sh_info = 1;
    }
    void SymbolTable::finalize(Generator& generator)
    {
        assert(!m_finalized);
        m_finalized = true;

        m_string_table.finalize(generator);

        generator.write_section(*m_section_index, m_stream);
    }
}
