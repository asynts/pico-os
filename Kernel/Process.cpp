#include <Kernel/Process.hpp>
#include <Kernel/Scheduler.hpp>
#include <Kernel/DynamicLoader.hpp>

namespace Kernel
{
    Process& Process::active_process()
    {
        auto& thread = Scheduler::the().active_thread();
        return thread.m_process.must();
    }

    Process& Process::create(StringView name, ElfWrapper elf)
    {
        Process process { name };

        Thread thread { String::format("Process: {}", name), move(process) };

        return Scheduler::the().create_thread(move(thread), [=] () mutable {
            dbgln("Loading executable for process '{}' from {}", name, elf.base());

            auto executable = load_executable_into_memory(elf);

            dbgln("Handing over execution to process '{}' at {}", name, executable.m_entry);

            asm volatile(
                "movs r0, #0;"
                "msr psp, r0;"
                "isb;"
                "movs r0, #0b11;"
                "msr control, r0;"
                "isb;"
                "mov r0, %1;"
                "mov sb, %2;"
                "blx %0;"
                :
                : "r"(executable.m_entry), "r"(executable.m_stack_base + executable.m_stack_size), "r"(executable.m_writable_base)
                : "r0");

            VERIFY_NOT_REACHED();
        }).m_process.must();
    }
}
