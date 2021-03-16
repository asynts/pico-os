#include <assert.h>

#include <fmt/format.h>

#include "Generator.hpp"
#include "RelocationTable.hpp"

namespace Elf
{
    RelocationTable::RelocationTable(Generator& generator, std::string_view name_suffix, size_t symtab_index, size_t target_index)
        : m_generator(generator)
        , m_name_suffix(name_suffix)
        , m_symtab_index(symtab_index)
        , m_target_index(target_index)
    {
        m_rel_index = m_generator.create_section(fmt::format(".rel{}", m_name_suffix), SHT_REL, 0);
        auto& rel_section = m_generator.section(*m_rel_index);
        rel_section.sh_entsize = sizeof(Elf32_Rel);
        rel_section.sh_link = m_symtab_index;
        rel_section.sh_info = m_target_index;

        m_rela_index = m_generator.create_section(fmt::format(".rela{}", m_name_suffix), SHT_RELA, 0);
        auto& rela_section = generator.section(*m_rela_index);
        rela_section.sh_entsize = sizeof(Elf32_Rela);
        rela_section.sh_link = m_symtab_index;
        rela_section.sh_info = m_target_index;
    }
    RelocationTable::~RelocationTable()
    {
        assert(m_finalized);
    }
    void RelocationTable::add_entry(Elf32_Rel relocation)
    {
        m_rel_stream.write_object(relocation);
    }
    void RelocationTable::add_entry(Elf32_Rela relocation)
    {
        m_rela_stream.write_object(relocation);
    }
    void RelocationTable::finalize()
    {
        assert(!m_finalized);
        m_finalized = true;

        m_generator.write_section(m_rel_index.value(), m_rel_stream);
        m_generator.write_section(m_rela_index.value(), m_rela_stream);
    }
}
