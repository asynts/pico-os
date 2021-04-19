#pragma once

#include <Std/Forward.hpp>
#include <Std/String.hpp>

#include <elf.h>

namespace Kernel
{
    using namespace Std;

    class ElfWrapper {
    public:
        explicit ElfWrapper(const u8 *base, StringView host_path)
            : m_base(base)
            , m_host_path(host_path)
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

        String m_host_path;

    private:
        const u8 *m_base;
        usize m_offset = 0;
    };

    struct LoadedExecutable {
        LoadedExecutable() = default;

        LoadedExecutable(const LoadedExecutable&) = delete;
        LoadedExecutable(LoadedExecutable&&) = default;

        u32 m_entry;

        u32 m_readonly_base;
        u32 m_readonly_size;

        u32 m_writable_base;
        u32 m_writable_size;

        u32 m_data_base;
        u32 m_text_base;
        u32 m_bss_base;

        u32 m_stack_base;
        u32 m_stack_size;

        String m_host_path;

        LoadedExecutable clone();
    };

    LoadedExecutable load_executable_into_memory(ElfWrapper);

    void hand_over_to_loaded_executable(const LoadedExecutable&);
}
