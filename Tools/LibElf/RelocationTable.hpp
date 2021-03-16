#pragma once

#include <string_view>
#include <optional>

#include <elf.h>

#include "MemoryStream.hpp"

namespace Elf
{
    class Generator;

    class RelocationTable {
    public:
        RelocationTable(Generator&, std::string_view name_suffix, size_t symtab_index, size_t target_index);

        void add_entry(Elf32_Rel);
        void add_entry(Elf32_Rela);

        void apply();

        size_t symtab_index() { return m_symtab_index; }
        size_t target_index() { return m_target_index; }
        size_t rel_index() { return m_rel_index.value(); }
        size_t rela_index() { return m_rela_index.value(); }

    private:
        Generator& m_generator;

        MemoryStream m_rel_stream;
        MemoryStream m_rela_stream;

        std::string_view m_name_suffix;

        size_t m_symtab_index;
        size_t m_target_index;

        std::optional<size_t> m_rel_index;
        std::optional<size_t> m_rela_index;
    };
}
