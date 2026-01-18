#pragma once

#include <string_view>
#include <optional>

#include <elf.h>

#include "StringTable.hpp"

namespace Elf
{
    class Generator;

    class SymbolTable {
    public:
        SymbolTable(std::string_view name_suffix);
        ~SymbolTable();

        size_t add_symbol(std::string_view name, Elf32_Sym symbol);
        size_t add_undefined_symbol(std::string_view name, Elf32_Sym symbol);

        void initialize(Generator& generator);
        void finalize(Generator& generator);

        std::string_view name() const { return m_name; }
        size_t section_index() const { return m_section_index.value(); }

    private:
        void create_undefined_symbol();

        bool m_finalized = false;
        size_t m_next_index = 0;

        StringTable m_string_table;
        MemoryStream m_stream;
        std::string m_name;
        std::optional<size_t> m_section_index;
    };
}
