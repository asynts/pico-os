#pragma once

#include <string_view>
#include <optional>

#include "MemoryStream.hpp"

namespace Elf
{
    class Generator;

    class StringTable {
    public:
        explicit StringTable(Generator&, std::string_view name);
        ~StringTable();

        size_t add_entry(std::string_view);

        void finalize();

        size_t strtab_index() const { return m_strtab_index.value(); }

    private:
        void create_undefined_entry();

        Generator& m_generator;
        bool m_finalized = false;

        MemoryStream m_strtab_stream;

        std::optional<size_t> m_strtab_index;
    };
}
