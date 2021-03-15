#pragma once

#include <string_view>
#include <optional>

#include "MemoryStream.hpp"

namespace Elf
{
    class Generator;

    class StringTable {
    public:
        explicit StringTable(std::string_view name_suffix);

        size_t add_entry(std::string_view);

        void apply(Generator&);

        size_t strtab_index() const { return m_strtab_index.value(); }

    private:
        MemoryStream m_strtab_stream;

        std::string_view m_name_suffix;
        bool m_applied = false;
        std::optional<size_t> m_strtab_index;
    };
}
