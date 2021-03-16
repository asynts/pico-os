#include <fmt/format.h>

#include "Generator.hpp"
#include "RelocationTable.hpp"

namespace Elf
{
    RelocationTable::RelocationTable(std::string_view name_suffix, size_t symtab_index, size_t target_index)
        : m_name_suffix(name_suffix)
        , m_symtab_index(symtab_index)
        , m_target_index(target_index)
    {
    }
    void RelocationTable::add_entry(Elf32_Rel relocation)
    {
        m_rel_stream.write_object(relocation);
    }
    void RelocationTable::add_entry(Elf32_Rela relocation)
    {
        m_rela_stream.write_object(relocation);
    }
    void RelocationTable::apply(Generator& generator)
    {
        size_t rel_index = generator.append_section(fmt::format(".rel{}", m_name_suffix), m_rel_stream, SHT_REL, 0);
        auto& rel_section = generator.section(rel_index);
        rel_section.sh_entsize = sizeof(Elf32_Rel);
        rel_section.sh_link = m_symtab_index;
        rel_section.sh_info = m_target_index;

        size_t rela_index = generator.append_section(fmt::format(".rela{}", m_name_suffix), m_rela_stream, SHT_RELA, 0);
        auto& rela_section = generator.section(rela_index);
        rela_section.sh_entsize = sizeof(Elf32_Rela);
        rela_section.sh_link = m_symtab_index;
        rela_section.sh_info = m_target_index;
    }
}
