#include <Std/Forward.hpp>

#include <Kernel/Interface/System.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/Process.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/Interface/Types.hpp>
#include <Kernel/GlobalMemoryAllocator.hpp>
#include <Kernel/Threads/Scheduler.hpp>

namespace Kernel
{
    SystemHandler::SystemHandler()
        : m_system_call_thread_name("Kernel: SystemHandler")
    {

    }

    extern "C"
    FullRegisterContext* syscall(FullRegisterContext *context)
    {
        auto& thread = Scheduler::the().active();

        thread.mark_blocked();
        thread.stash_context(*context);

        // We are not allowed to allocate here and must use a constant string.
        auto worker_thread = Thread::construct(SystemHandler::the().get_system_call_thread_name());

        worker_thread->m_privileged = true;
        worker_thread->setup_context([&thread, context] {
            i32 return_value = thread.syscall(context->r0.syscall(), context->r1, context->r2, context->r3);

            if (context->r0.syscall() == _SC_exit)
                VERIFY(thread.m_die_at_next_opportunity);

            thread.m_stashed_context.must()->r0.m_storage = bit_cast<u32>(return_value);

            thread.mark_unblocked();
        });
        Scheduler::the().add_thread(worker_thread);

        Thread& next_thread = Scheduler::the().schedule();
        context = &next_thread.unstash_context();

        return context;
    }
}
