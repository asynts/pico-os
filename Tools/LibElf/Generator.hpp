#pragma once

#include <string_view>
#include <vector>

#include <elf.h>

#include "MemoryStream.hpp"

namespace Elf
{
    class Generator {
    public:
        Generator();

        Elf32_Shdr& section(size_t index) { return m_sections[index]; }

        size_t append_section(
            std::string_view name,
            MemoryStream& stream,
            Elf32_Word type = SHT_PROGBITS,
            Elf32_Word flags = SHF_ALLOC);

        size_t create_section(
            std::string_view name,
            Elf32_Word type = SHT_PROGBITS,
            Elf32_Word flags = SHF_ALLOC);

        void write_section(size_t section_index, MemoryStream&);

        MemoryStream finalize() &&;

    private:
        size_t append_section_name(std::string_view);

        size_t append_shstrtab_section();

        void encode_sections(size_t& section_offset, size_t& shstrtab_section_index);
        void encode_header(size_t section_offset, size_t shstrtab_section_index);

        MemoryStream m_stream;

        bool m_finalized = false;
        std::vector<Elf32_Shdr> m_sections;
        MemoryStream m_shstrtab_stream;
    };
}
