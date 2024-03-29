#pragma once

#include <Std/Forward.hpp>
#include <Std/String.hpp>

#include <Kernel/MPU.hpp>
#include <Kernel/StackWrapper.hpp>

#include <elf.h>

namespace Kernel
{
    constexpr bool debug_loader = false;

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

        ImmutableString m_host_path;

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

        ImmutableString m_host_path;
    };

    LoadedExecutable load_executable_into_memory(ElfWrapper, Thread&);

    void setup_mpu(Vector<MPU::Region>&);

    void hand_over_to_loaded_executable(const LoadedExecutable&, StackWrapper, Vector<MPU::Region>&, i32 argc, char **argv, char **envp);
}
