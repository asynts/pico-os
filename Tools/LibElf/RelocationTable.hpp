#pragma once

#include <string_view>

#include <elf.h>

#include "MemoryStream.hpp"

namespace Elf
{
    class Generator;

    class RelocationTable {
    public:
        RelocationTable(std::string_view name_suffix, size_t symtab_index, size_t target_index);

        void add_entry(Elf32_Rel);
        void add_entry(Elf32_Rela);

        void apply(Generator&);

    private:
        MemoryStream m_rel_stream;
        MemoryStream m_rela_stream;

        std::string_view m_name_suffix;

        size_t m_symtab_index;
        size_t m_target_index;
    };
}
