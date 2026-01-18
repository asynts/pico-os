#include <assert.h>
#include <elf.h>

#include "StringTable.hpp"
#include "Generator.hpp"

namespace Elf
{
    StringTable::StringTable(std::string name)
        : m_name(name)
    {
        create_undefined_entry();
    }
    StringTable::~StringTable()
    {
        assert(m_finalized);
    }
    void StringTable::create_undefined_entry()
    {
        m_stream.write_object<uint8_t>(0);
    }
    size_t StringTable::add_entry(std::string_view value)
    {
        size_t offset = m_stream.write_bytes({ (const uint8_t*)value.data(), value.size() });
        m_stream.write_object<uint8_t>(0);
        return offset;
    }
    void StringTable::initialize(Generator& generator)
    {
        m_section_index = generator.create_section(m_name, SHT_STRTAB, 0);
    }
    void StringTable::finalize(Generator& generator)
    {
        assert(!m_finalized);
        m_finalized = true;

        generator.write_section(m_section_index.value(), m_stream);
    }
}
