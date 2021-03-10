#include <pico/stdio.h>
#include <pico/printf.h>
#include <pico/sync.h>

#include <Std/Forward.hpp>

#include <elf.h>

extern "C" {
    extern u8 embedded_shell_binary_start[];
    extern u8 embedded_shell_binary_end[];
    extern u8 embedded_shell_binary_size[];
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
    u32 m_entry;

    u32 m_readonly_base;
    u32 m_writable_base;

    u32 m_data_base;
    u32 m_text_base;

    u32 m_stack_base;
    u32 m_stack_size;
};

// FIXME: I want this to be a watchpoint, or a function, but not whatever this is.
extern "C"
{
    volatile LoadedExecutable *volatile executable_for_debugger;
    [[gnu::noinline]]
    void inform_debugger_about_executable()
    {
        asm volatile("nop");
    }
}

LoadedExecutable load_executable_into_memory(ElfWrapper elf)
{
    LoadedExecutable executable;

    printf("Loading executable from %p\n", elf.base());

    assert(elf.header()->e_phnum == 3);
    assert(elf.segments()[2].p_type == PT_ARM_EXIDX);

    Elf32_Phdr& text_segment = elf.segments()[0];
    assert(text_segment.p_type == PT_LOAD);
    assert(text_segment.p_flags == PF_R | PF_X);

    Elf32_Phdr& data_segment = elf.segments()[1];
    assert(data_segment.p_type == PT_LOAD);
    assert(data_segment.p_flags == PF_R | PF_W);

    executable.m_readonly_base = elf.base_as_u32() + text_segment.p_offset;
    printf("Putting readonly segment at %p (inplace)\n", executable.m_readonly_base);

    u8 *data = new u8[data_segment.p_memsz];
    assert(data != nullptr);

    executable.m_writable_base = u32(data);
    printf("Putting writable segment at %p (allocated)\n", executable.m_writable_base);

    __builtin_memcpy(data, elf.base() + data_segment.p_offset, data_segment.p_filesz);

    assert(data_segment.p_memsz >= data_segment.p_filesz);
    __builtin_memset(data + data_segment.p_filesz, 0, data_segment.p_memsz - data_segment.p_filesz);

    assert(elf.header()->e_entry >= text_segment.p_vaddr);
    assert(elf.header()->e_entry - text_segment.p_vaddr < text_segment.p_memsz);
    executable.m_entry = executable.m_readonly_base + (elf.header()->e_entry - text_segment.p_vaddr);
    printf("Putting entry point at %p\n", executable.m_entry);

    for (usize section_index = 1; section_index < elf.header()->e_shnum; ++section_index) {
        Elf32_Shdr& section = elf.sections()[section_index];

        if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".stack") == 0) {
            executable.m_stack_base = executable.m_writable_base + section.sh_addr;
            executable.m_stack_size = 0x10000;
            continue;
        }

        if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".data") == 0) {
            executable.m_data_base = executable.m_writable_base + section.sh_addr;
            continue;
        }

        if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".text") == 0) {
            executable.m_text_base = executable.m_readonly_base + section.sh_addr;
            continue;
        }
    }
    assert(executable.m_text_base);
    assert(executable.m_data_base);
    assert(executable.m_stack_base);

    printf("Found text segment at %p in readonly segment\n", executable.m_text_base);
    printf("Found data segment at %p in readonly segment\n", executable.m_data_base);
    printf("Found stack segment at %p in readonly segment\n", executable.m_stack_base);

    executable_for_debugger = &executable;
    inform_debugger_about_executable();

    return executable;
}

void load_and_execute_shell()
{
    ElfWrapper elf { reinterpret_cast<u8*>(embedded_shell_binary_start) };
    LoadedExecutable executable = load_executable_into_memory(elf);

    printf("Handing over execution to new process\n");

    // FIXME: Setup PIC register. (SB)

    asm volatile(
        "movs r0, #0;"
        "msr psp, r0;"
        "isb;"
        "movs r0, #0b11;"
        "msr control, r0;"
        "isb;"
        "mov r0, %1;"
        "mov sb, %2;"
        "bkpt #0;"
        "blx %0;"
        :
        : "r"(executable.m_entry), "r"(executable.m_stack_base + executable.m_stack_size), "r"(executable.m_writable_base)
        : "r0");

    panic("Process returned, it shouldn't have\n");
}

int main() {
    stdio_init_all();

    printf("\033[1mBOOT\033[0m\n");

    load_and_execute_shell();

    for(;;)
        __wfi();
}
