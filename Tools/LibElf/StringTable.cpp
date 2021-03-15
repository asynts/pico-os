#include <assert.h>
#include <elf.h>

#include <fmt/format.h>

#include "StringTable.hpp"
#include "Generator.hpp"

namespace Elf
{
    StringTable::StringTable(std::string_view name_suffix)
        : m_name_suffix(name_suffix)
    {
        add_entry("");
    }
    size_t StringTable::add_entry(std::string_view name)
    {
        size_t offset = m_strtab_stream.write_bytes({ (const uint8_t*)name.data(), name.size() });
        m_strtab_stream.write_object<uint8_t>(0);
        return offset;
    }
    void StringTable::apply(Generator& generator)
    {
        assert(!m_applied);
        m_applied = true;

        // We do this in two steps, because this might be the .shstrtab string table
        m_strtab_index = generator.create_section(fmt::format(".strtab.{}", m_name_suffix), SHT_STRTAB, 0);
        generator.write_section(m_strtab_index.value(), m_strtab_stream);
    }
}
