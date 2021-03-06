#include <pico/stdio.h>
#include <pico/printf.h>
#include <pico/sync.h>

#include <Std/Forward.hpp>

#include <elf.h>

extern "C" {
    extern u8 _binary_Shell_elf_start[];
    extern u8 _binary_Shell_elf_end[];
    extern u8 _binary_Shell_elf_size[];
}

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
    u32 m_entry_address;
    u32 m_text_base;
    u32 m_data_base;
    u32 m_stack_base;
    u32 m_stack_top;
    u32 m_stack_size;
};

LoadedExecutable load_executable_into_memory(ElfWrapper elf)
{
    printf("Loading executable from %p\n", elf.base());

    assert(elf.header()->e_phnum == 3);
    assert(elf.segments()[0].p_type == PT_ARM_EXIDX);

    Elf32_Phdr& text_segment = elf.segments()[1];
    assert(text_segment.p_type == PT_LOAD);
    assert(text_segment.p_flags == PF_R | PF_X);

    Elf32_Phdr& data_segment = elf.segments()[2];
    assert(data_segment.p_type == PT_LOAD);
    assert(data_segment.p_flags == PF_R | PF_W);

    u32 text_base = elf.base_as_u32() + text_segment.p_offset;
    printf("Putting text segment at %p (inplace)\n", text_base);

    u8 *data = new u8[data_segment.p_memsz];
    assert(data != nullptr);

    u32 data_base = u32(data);
    printf("Putting data segment at %p (allocated)\n", data_base);

    __builtin_memcpy(data, elf.base() + data_segment.p_offset, data_segment.p_filesz);

    assert(data_segment.p_memsz >= data_segment.p_filesz);
    __builtin_memset(data + data_segment.p_filesz, 0, data_segment.p_memsz - data_segment.p_filesz);

    assert(elf.header()->e_entry >= text_segment.p_vaddr);
    assert(elf.header()->e_entry - text_segment.p_vaddr < text_segment.p_memsz);
    u32 entry_address = text_base + (elf.header()->e_entry - text_segment.p_vaddr);

    printf("Putting entry point at %p\n", entry_address);

    u32 stack_size = 0x1000;
    u32 stack_base = u32(new u8[stack_size]);
    u32 stack_top = stack_base + stack_top;
    printf("Putting stack top at %p\n", stack_top);

    return {
        .m_entry_address = entry_address,
        .m_text_base = text_base,
        .m_data_base = data_base,
        .m_stack_base = stack_base,
        .m_stack_top = stack_top,
        .m_stack_size = stack_size,
    };
}

void load_and_execute_shell()
{
    ElfWrapper elf { reinterpret_cast<u8*>(_binary_Shell_elf_start) };
    LoadedExecutable executable = load_executable_into_memory(elf);

    // FIXME: Inform the debugger about the loaded executable.
    printf("Finished loading executable, informing debugger\n");
    __breakpoint();

    // FIXME: Setup PIC register. (SB)

    printf("Handing over execution to new process\n");

    // Switch to process stack pointer and execute unprivileged.
    asm volatile(
        "msr psp, %0;"
        "isb;"
        "movs r0, #0b11;"
        "msr control, r0;"
        "isb;"
        "blx %1;"
        :
        : "r"(executable.m_stack_top), "r"(executable.m_entry_address));

    panic("Process returned, it shouldn't have\n");
}

int main() {
    stdio_init_all();

    printf("\033[1mBOOT\033[0m\n");

    load_and_execute_shell();

    for(;;)
        __wfi();
}
