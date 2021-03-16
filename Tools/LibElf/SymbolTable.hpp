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
        SymbolTable(Generator&, std::string_view name_suffix);

        size_t add_symbol(std::string_view name, Elf32_Sym);
        size_t add_undefined_symbol(std::string_view name, Elf32_Sym);

        void apply();
    
        size_t symtab_index() { return m_symtab_index.value(); }

    private:
        void create_undefined_symbol();

        Generator& m_generator;

        StringTable m_string_table;
        MemoryStream m_symtab_stream;

        std::string_view m_name_suffix;
        size_t m_next_index = 0;
        bool m_applied = false;

        std::optional<size_t> m_symtab_index;
    };
}
