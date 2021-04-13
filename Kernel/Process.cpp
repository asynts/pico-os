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

        return Scheduler::the().create_thread(move(thread), [name, elf] () mutable {
            dbgln("Loading executable for process '{}' from {}", name, elf.base());

            auto& process = Process::active_process();

            process.m_executable = load_executable_into_memory(elf);

            dbgln("Handing over execution to process '{}' at {}", name, process.m_executable.must().m_entry);

            hand_over_to_loaded_executable(process.m_executable.must());

            VERIFY_NOT_REACHED();
        }).m_process.must();
    }

    i32 Process::sys$read(i32 fd, u8 *buffer, usize count)
    {
        if (fd > 2)
            dbgln("[Process::sys$read] fd={} buffer={} count={}", fd, buffer, count);

        auto& handle = get_file_handle(fd);
        return handle.read({ buffer, count }).must();
    }

    i32 Process::sys$write(i32 fd, const u8 *buffer, usize count)
    {
        if (fd > 2)
            dbgln("[Process::sys$write] fd={} buffer={} count={}", fd, buffer, count);

        auto& handle = get_file_handle(fd);
        return handle.write({ buffer, count }).must();
    }
}
