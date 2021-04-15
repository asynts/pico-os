#include <Std/Format.hpp>

#include <Kernel/DynamicLoader.hpp>
#include <Kernel/Scheduler.hpp>
#include <Kernel/GlobalMemoryAllocator.hpp>

namespace Kernel
{
    LoadedExecutable load_executable_into_memory(ElfWrapper elf)
    {
        LoadedExecutable executable;
        executable.m_host_path = elf.m_host_path;

        VERIFY(elf.header()->e_phnum == 3);
        VERIFY(elf.segments()[2].p_type == PT_ARM_EXIDX);

        auto& readonly_segment = elf.segments()[0];
        VERIFY(readonly_segment.p_type == PT_LOAD);
        VERIFY(readonly_segment.p_flags == PF_R | PF_X);

        auto& writable_segment = elf.segments()[1];
        VERIFY(writable_segment.p_type == PT_LOAD);
        VERIFY(writable_segment.p_flags == PF_R | PF_W);

        executable.m_readonly_base = elf.base_as_u32() + readonly_segment.p_offset;
        executable.m_readonly_size = readonly_segment.p_memsz;

        u8 *writable = new u8[writable_segment.p_memsz];
        VERIFY(writable != nullptr);
        executable.m_writable_base = u32(writable);
        executable.m_writable_size = writable_segment.p_memsz;

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
                executable.m_stack_size = 0x1100;
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

        dbgln("GDB: add-symbol-file {} -s .text {} -s .data {} -s .bss {}",
            executable.m_host_path,
            executable.m_text_base,
            executable.m_data_base,
            executable.m_bss_base);

        return move(executable);
    }

    LoadedExecutable LoadedExecutable::clone()
    {
        LoadedExecutable copy;

        copy.m_writable_size = m_writable_size;
        copy.m_writable_base = u32(new u8[m_writable_size]);
        memcpy((void*)copy.m_writable_base, (void*)m_writable_base, m_writable_size);

        copy.m_readonly_size = m_readonly_size;
        copy.m_readonly_base = m_readonly_base;

        copy.m_entry = copy.m_readonly_base + (m_entry - m_readonly_base);

        copy.m_data_base = copy.m_writable_base + (m_data_base - m_writable_base);
        copy.m_text_base = copy.m_readonly_base + (m_text_base - m_readonly_base);
        copy.m_bss_base = copy.m_writable_base + (m_bss_base - m_writable_base);

        copy.m_stack_size = m_stack_size;
        copy.m_stack_base = u32(new u8[m_stack_size]);
        memcpy((void*)copy.m_stack_base, (void*)m_stack_base, m_stack_size);

        return copy;
    }

    // FIXME: This is a huge mess

    void hand_over_to_loaded_executable(const LoadedExecutable& executable)
    {
        asm volatile(
            "movs r0, #0;"
            "msr psp, r0;"
            "isb;"
            "movs r0, #0b11;"
            "msr control, r0;"
            "isb;"
            :
            :
            : "r0");

        Scheduler::the().active_thread().m_privileged = false;

        asm volatile(
            "mov r0, %1;"
            "mov sb, %2;"
            "blx %0;"
            :
            : "r"(executable.m_entry), "r"(executable.m_stack_base + executable.m_stack_size), "r"(executable.m_writable_base)
            : "r0");

        VERIFY_NOT_REACHED();
    }

    void hand_over_to_forked_executable(const LoadedExecutable& executable)
    {
        // Setup stack for process
        asm volatile(
            "msr psp, %0;"
            "isb;"
            :
            : "r"(executable.m_stack_base + executable.m_stack_size));

        // Setup the static base register
        asm volatile(
            "mov sb, %0;"
            :
            : "r"(executable.m_writable_base));

        Scheduler::the().active_thread().m_privileged = false;

        // Drop privileges
        asm volatile(
            "movs r0, #0b11;"
            "msr control, r0;"
            :
            :
            : "r0");

        // Hand over execution
        asm volatile(
            "blx %0;"
            :
            : "r"(executable.m_entry));

        VERIFY_NOT_REACHED();
    }
}
