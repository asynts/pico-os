#pragma once

#include <string_view>
#include <optional>

#include <elf.h>

#include "StringTable.hpp"

namespace Elf
{
    class SymbolTable {
    public:
        explicit SymbolTable(std::string_view name_suffix, size_t target_index);

        size_t add_symbol(std::string_view name, Elf32_Sym);

        void add_relocation(Elf32_Rel);
        void add_relocation(Elf32_Rela);

        void apply(Generator& generator);

    private:
        StringTable m_string_table;
        MemoryStream m_symtab_stream;
        MemoryStream m_rel_stream;
        MemoryStream m_rela_stream;

        std::string_view m_name_suffix;
        size_t m_next_index = 0;
        bool m_applied = false;

        size_t m_target_index;
        std::optional<size_t> m_symtab_index;
        std::optional<size_t> m_rel_index;
        std::optional<size_t> m_rela_index;
    };
}
