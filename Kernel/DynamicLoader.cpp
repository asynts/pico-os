#include <Kernel/DynamicLoader.hpp>
#include <Std/Format.hpp>

// FIXME: I want this to be a watchpoint, or a function, but not whatever this is.
// FIXME: The debugger assumes that this is the shell binary.
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

    VERIFY(elf.header()->e_phnum == 3);
    VERIFY(elf.segments()[2].p_type == PT_ARM_EXIDX);

    auto& readonly_segment = elf.segments()[0];
    VERIFY(readonly_segment.p_type == PT_LOAD);
    VERIFY(readonly_segment.p_flags == PF_R | PF_X);

    auto& writable_segment = elf.segments()[1];
    VERIFY(writable_segment.p_type == PT_LOAD);
    VERIFY(writable_segment.p_flags == PF_R | PF_W);

    executable.m_readonly_base = elf.base_as_u32() + readonly_segment.p_offset;

    u8 *writable = new u8[writable_segment.p_memsz];
    VERIFY(writable != nullptr);
    executable.m_writable_base = u32(writable);

    __builtin_memcpy(writable, elf.base() + writable_segment.p_offset, writable_segment.p_filesz);

    VERIFY(writable_segment.p_memsz >= writable_segment.p_filesz);
    __builtin_memset(writable + writable_segment.p_filesz, 0, writable_segment.p_memsz - writable_segment.p_filesz);

    VERIFY(elf.header()->e_entry >= readonly_segment.p_vaddr);
    VERIFY(elf.header()->e_entry - readonly_segment.p_vaddr < readonly_segment.p_memsz);
    executable.m_entry = executable.m_readonly_base + (elf.header()->e_entry - readonly_segment.p_vaddr);

    executable.m_text_base = 0;
    executable.m_data_base = 0;
    executable.m_stack_base = 0;
    executable.m_bss_base = 0;
    for (usize section_index = 1; section_index < elf.header()->e_shnum; ++section_index) {
        auto& section = elf.sections()[section_index];

        if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".stack") == 0) {
            executable.m_stack_base = executable.m_writable_base + section.sh_addr;
            executable.m_stack_size = 0x10000;
            continue;
        }
        if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".data") == 0) {
            executable.m_data_base = executable.m_writable_base + section.sh_addr;
            continue;
        }
        if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".bss") == 0) {
            executable.m_bss_base = executable.m_writable_base + section.sh_addr;
            continue;
        }

        if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".text") == 0) {
            executable.m_text_base = executable.m_readonly_base + section.sh_addr;
            continue;
        }
    }
    VERIFY(executable.m_text_base);
    VERIFY(executable.m_data_base);
    VERIFY(executable.m_stack_base);
    VERIFY(executable.m_bss_base);

    executable_for_debugger = &executable;
    inform_debugger_about_executable();

    return executable;
}
