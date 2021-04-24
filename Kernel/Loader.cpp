#include <Std/Format.hpp>

#include <Kernel/Loader.hpp>
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

        dbgln("Copying {} bytes into writeable segment", writable_segment.p_filesz);
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
                executable.m_stack_base = executable.m_writable_base + (section.sh_addr - writable_segment.p_vaddr);
                executable.m_stack_size = section.sh_size;

                // FIXME: I don't think we need this anymore with our own LibC
                ASSERT(executable.m_stack_size == 0x1100);

                continue;
            }
            if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".data") == 0) {
                executable.m_data_base = executable.m_writable_base + (section.sh_addr - writable_segment.p_vaddr);
                continue;
            }
            if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".bss") == 0) {
                executable.m_bss_base = executable.m_writable_base + (section.sh_addr - writable_segment.p_vaddr);
                continue;
            }

            if (__builtin_strcmp(elf.section_name_base() + section.sh_name, ".text") == 0) {
                executable.m_text_base = executable.m_readonly_base + (section.sh_addr - readonly_segment.p_vaddr);
                continue;
            }
        }
        VERIFY(executable.m_text_base);
        VERIFY(executable.m_data_base);
        VERIFY(executable.m_stack_base);
        VERIFY(executable.m_bss_base);

        dbgln("Loaded executable:");
        dbgln("  m_readonly_base={}", executable.m_readonly_base);
        dbgln("  m_writable_base={}", executable.m_writable_base);

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

    void hand_over_to_loaded_executable(const LoadedExecutable& executable)
    {
        StackWrapper stack { { (u8*)executable.m_stack_base, executable.m_stack_size } };
        stack.align(8);

        // We could reach this point in thread or handler mode
        u32 ipsr;
        asm ("mrs %0, ipsr;"
             "isb;"
            : "=r"(ipsr));
        bool is_thread_mode = (ipsr == 0);

        if (is_thread_mode) {
            // Setup stack pointer
            asm volatile(
                "msr psp, %0;"
                "isb;"
                :
                : "r"(stack.top()));

            // Drop privileges
            asm volatile(
                "msr control, %0;"
                "isb;"
                :
                : "r"(0b11));
            Scheduler::the().active_thread().m_privileged = false;

            asm volatile(
                "mov r0, %0;"
                "mov sb, %1;"
                "blx %2;"
                :
                : "r"(stack.top()), "r"(executable.m_writable_base), "r"(executable.m_entry)
                : "r0", "sb");
        } else {
            constexpr u32 xpsr_thumb_mode = 1 << 24;

            auto *context = stack.push_value(ExceptionRegisterContext{});
            context->pc.m_storage = executable.m_entry;
            context->r0.m_storage = u32(context);
            context->xpsr.m_storage = xpsr_thumb_mode;
            Scheduler::the().active_thread().m_context.clear();

            // Setup stack pointer
            asm volatile(
                "msr psp, %0;"
                "isb;"
                :
                : "r"(context));

            // Drop privileges
            asm volatile(
                "msr control, %0;"
                "isb;"
                :
                : "r"(0b01));
            Scheduler::the().active_thread().m_privileged = false;

            // Hand over to executable
            asm volatile(
                "mov r9, %0;"
                "bx %1;"
                :
                : "r"(executable.m_writable_base), "r"(0xfffffffd)
                : "r9");
        }

        VERIFY_NOT_REACHED();
    }
}
