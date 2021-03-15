#include <assert.h>
#include <elf.h>

#include "StringTable.hpp"
#include "Generator.hpp"

namespace Elf
{
    StringTable::StringTable(const std::string& name)
        : m_name(name)
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
        m_strtab_index = generator.create_section(m_name, SHT_STRTAB, 0);
        generator.write_section(m_strtab_index.value(), m_strtab_stream);
    }
}
