#include <Kernel/DynamicLoader.hpp>
#include <Std/Debug.hpp>
#include <assert.h>

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

    dbgprintf("Loading executable from %p\n", elf.base());

    assert(elf.header()->e_phnum == 3);
    assert(elf.segments()[2].p_type == PT_ARM_EXIDX);

    Elf32_Phdr& text_segment = elf.segments()[0];
    assert(text_segment.p_type == PT_LOAD);
    assert(text_segment.p_flags == PF_R | PF_X);

    Elf32_Phdr& data_segment = elf.segments()[1];
    assert(data_segment.p_type == PT_LOAD);
    assert(data_segment.p_flags == PF_R | PF_W);

    executable.m_readonly_base = elf.base_as_u32() + text_segment.p_offset;
    dbgprintf("Putting readonly segment at %p (inplace)\n", executable.m_readonly_base);

    u8 *data = new u8[data_segment.p_memsz];
    assert(data != nullptr);

    executable.m_writable_base = u32(data);
    dbgprintf("Putting writable segment at %p (allocated)\n", executable.m_writable_base);

    __builtin_memcpy(data, elf.base() + data_segment.p_offset, data_segment.p_filesz);

    assert(data_segment.p_memsz >= data_segment.p_filesz);
    __builtin_memset(data + data_segment.p_filesz, 0, data_segment.p_memsz - data_segment.p_filesz);

    assert(elf.header()->e_entry >= text_segment.p_vaddr);
    assert(elf.header()->e_entry - text_segment.p_vaddr < text_segment.p_memsz);
    executable.m_entry = executable.m_readonly_base + (elf.header()->e_entry - text_segment.p_vaddr);
    dbgprintf("Putting entry point at %p\n", executable.m_entry);

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

    dbgprintf("Found text segment at %p in readonly segment\n", executable.m_text_base);
    dbgprintf("Found data segment at %p in writable segment\n", executable.m_data_base);
    dbgprintf("Found stack segment at %p in readonly segment\n", executable.m_stack_base);

    executable_for_debugger = &executable;
    inform_debugger_about_executable();

    return executable;
}
