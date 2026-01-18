#pragma once

#include <string_view>
#include <optional>

#include "MemoryStream.hpp"

namespace Elf
{
    class Generator;

    class StringTable {
    public:
        explicit StringTable(std::string name);
        ~StringTable();

        size_t add_entry(std::string_view value);

        void initialize(Generator& generator);
        void finalize(Generator& generator);

        std::string_view name() const { return m_name; }
        size_t section_index() const { return m_section_index.value(); }

    private:
        void create_undefined_entry();

        std::string m_name;
        bool m_finalized = false;
        MemoryStream m_stream;
        std::optional<size_t> m_section_index;
    };
}
