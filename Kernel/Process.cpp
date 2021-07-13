#include <Std/OwnPtr.hpp>

#include <Kernel/Process.hpp>
#include <Kernel/Threads/Scheduler.hpp>
#include <Kernel/Loader.hpp>
#include <Kernel/Interface/System.hpp>
#include <Kernel/FileSystem/FlashFileSystem.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>

namespace Kernel
{
    Process& Process::active()
    {
        auto& thread = Scheduler::the().active();
        return thread.m_process.must();
    }

    Process& Process::create(StringView name, ElfWrapper elf)
    {
        Vector<String> arguments;
        arguments.append(name);

        Vector<String> variables;

        return Process::create(name, elf, arguments, variables);
    }
    Process& Process::create(StringView name, ElfWrapper elf, const Vector<String>& arguments, const Vector<String>& variables)
    {
        auto process = Process::construct(name);

        auto thread = Thread::construct(String::format("Process: {}", name));

        thread->m_process = process;

        // FIXME: Is this still required?
        thread->m_privileged = true;

        thread->setup_context([arguments, variables, name, elf]() mutable {
            dbgln("Loading executable for process '{}' from {}", name, elf.base());

            auto& process = Process::active();

            process.m_executable = load_executable_into_memory(elf, Thread::active());
            auto& executable = process.m_executable.must();

            StackWrapper stack { { (u8*)executable.m_stack_base, executable.m_stack_size } };

            auto push_cstring_array = [&stack] (const Vector<String>& array) {
                Vector<char*> pointers;
                for (auto& value : array.iter()) {
                    pointers.append(stack.push_cstring(value.cstring()));
                }
                char **pointer = (char**)stack.push_value(nullptr);
                for (size_t i = 0; i < pointers.size(); ++i) {
                    pointer = stack.push_value(pointers[pointers.size() - i - 1]);
                }

                return pointer;
            };

            int argc = arguments.size();
            char **argv = push_cstring_array(arguments);
            char **envp = push_cstring_array(variables);

            dbgln("[Process::create] argc={} argv={} envp={}", argc, argv, envp);

            dbgln("[Process::create] argv:");
            for (char **value = argv; *value; ++value) {
                dbgln("  {}: {}", *value, StringView { *value });
            }

            dbgln("[Process::create] envp:");
            for (char **value = envp; *value; ++value) {
                dbgln("  {}: {}", *value, StringView { *value });
            }

            auto& thread = Scheduler::the().active();

            VERIFY(__builtin_popcount(executable.m_writable_size) == 1);
            VERIFY(executable.m_writable_base % executable.m_writable_size == 0);
            auto& ram_region = thread.m_regions.append({});
            ram_region.rbar.region = 0;
            ram_region.rbar.valid = 0;
            ram_region.rbar.addr = executable.m_writable_base >> 5;
            ram_region.rasr.enable = 1;
            ram_region.rasr.size = MPU::compute_size(executable.m_writable_size);
            ram_region.rasr.srd = 0b00000000;
            ram_region.rasr.attrs_b = 1;
            ram_region.rasr.attrs_c = 1;
            ram_region.rasr.attrs_s = 1;
            ram_region.rasr.attrs_tex = 0b000;
            ram_region.rasr.attrs_ap = 0b011;
            ram_region.rasr.attrs_xn = 1;

            dbgln("[Process::create] ram_region.rbar={}", ram_region.rbar.raw);

            auto& rom_region = thread.m_regions.append({});
            rom_region.rbar.region = 0;
            rom_region.rbar.valid = 0;
            rom_region.rbar.addr = 0x00000000 >> 5;
            rom_region.rasr.enable = 1;
            rom_region.rasr.size = 13;
            rom_region.rasr.srd = 0b00000000;
            rom_region.rasr.attrs_b = 1;
            rom_region.rasr.attrs_c = 1;
            rom_region.rasr.attrs_s = 1;
            rom_region.rasr.attrs_tex = 0b000;
            rom_region.rasr.attrs_ap = 0b111;
            rom_region.rasr.attrs_xn = 0;

            dbgln("[Process::create] rom_region.rbar={}", rom_region.rbar.raw);

            dbgln("Handing over execution to process '{}' at {}", name, process.m_executable.must().m_entry);
            dbgln("  Got argv={} and envp={}", argv, envp);

            hand_over_to_loaded_executable(process.m_executable.must(), stack, thread.m_regions, argc, argv, envp);

            VERIFY_NOT_REACHED();
        });

        Scheduler::the().add_thread(thread);

        return *process;
    }
}
