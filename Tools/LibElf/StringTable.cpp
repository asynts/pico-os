#include <assert.h>
#include <elf.h>

#include "StringTable.hpp"
#include "Generator.hpp"

namespace Elf
{
    StringTable::StringTable(Generator& generator, std::string_view name)
        : m_generator(generator)
    {
        create_undefined_entry();

        m_strtab_index = m_generator.create_section(name, SHT_STRTAB, 0);
    }
    void StringTable::create_undefined_entry()
    {
        m_strtab_stream.write_object<uint8_t>(0);
    }
    size_t StringTable::add_entry(std::string_view name)
    {
        size_t offset = m_strtab_stream.write_bytes({ (const uint8_t*)name.data(), name.size() });
        m_strtab_stream.write_object<uint8_t>(0);
        return offset;
    }
    void StringTable::apply()
    {
        m_generator.write_section(m_strtab_index.value(), m_strtab_stream);
    }
}
