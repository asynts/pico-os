#pragma once

#include <Std/Forward.hpp>

#include <elf.h>

class ElfWrapper {
public:
    explicit ElfWrapper(const u8 *base)
        : m_base(base)
    {
    }

    const u8* base() { return m_base; }
    u32 base_as_u32() { return reinterpret_cast<u32>(m_base); }

    usize offset() { return m_offset; }

    const Elf32_Ehdr* header() { return reinterpret_cast<const Elf32_Ehdr*>(m_base); }
    const Elf32_Phdr* segments() { return reinterpret_cast<const Elf32_Phdr*>(m_base + header()->e_phoff); }
    const Elf32_Shdr* sections() { return reinterpret_cast<const Elf32_Shdr*>(m_base + header()->e_shoff); }

    const char* section_name_base() { return reinterpret_cast<const char*>(m_base + sections()[header()->e_shstrndx].sh_offset); }

    void consume(usize size)
    {
        m_offset += size;
    }

private:
    const u8 *m_base;
    usize m_offset = 0;
};

struct LoadedExecutable {
    u32 m_entry;

    u32 m_readonly_base;
    u32 m_writable_base;

    u32 m_data_base;
    u32 m_text_base;
    u32 m_bss_base;

    u32 m_stack_base;
    u32 m_stack_size;
};

LoadedExecutable load_executable_into_memory(ElfWrapper);
