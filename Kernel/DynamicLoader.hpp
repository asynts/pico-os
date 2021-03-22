#include <elf.h>
#include <Std/Forward.hpp>

class ElfWrapper {
public:
    explicit ElfWrapper(u8 *base)
        : m_base(base)
    {
    }

    u8* base() { return m_base; }
    u32 base_as_u32() { return reinterpret_cast<u32>(m_base); }

    usize offset() { return m_offset; }

    Elf32_Ehdr* header() { return reinterpret_cast<Elf32_Ehdr*>(m_base); }
    Elf32_Phdr* segments() { return reinterpret_cast<Elf32_Phdr*>(m_base + header()->e_phoff); }
    Elf32_Shdr* sections() { return reinterpret_cast<Elf32_Shdr*>(m_base + header()->e_shoff); }

    char* section_name_base() { return reinterpret_cast<char*>(m_base + sections()[header()->e_shstrndx].sh_offset); }

    void append(void *data, usize size)
    {
        __builtin_memcpy(m_base + m_offset, data, size);
        m_offset += size;
    }

    void consume(usize size)
    {
        m_offset += size;
    }

private:
    u8 *m_base;
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
