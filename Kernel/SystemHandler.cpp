#include <Std/Forward.hpp>

#include <Kernel/Interface/System.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/Process.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/Interface/Types.hpp>
#include <Kernel/Threads/Scheduler.hpp>

namespace Kernel
{
    constexpr bool debug_syscall = false;

    extern "C"
    FullRegisterContext* syscall(FullRegisterContext *context)
    {
        if (debug_syscall)
            dbgln("[syscall] syscall={}", context->r0.syscall());

        auto& thread = Scheduler::the().active();

        thread.block();
        thread.stash_context(*context);

        auto worker_thread = Thread::construct(String::format("Worker: '{}' ({}): syscall={}", thread.m_name, &thread, context->r0.syscall()));
        worker_thread->m_privileged = true;
        worker_thread->setup_context([&thread, context] {
            i32 return_value = thread.syscall(context->r0.syscall(), context->r1, context->r2, context->r3);
            thread.m_stashed_context.must()->r0.m_storage = bit_cast<u32>(return_value);

            thread.unblock();
        });
        Scheduler::the().add_thread(worker_thread);

        Thread& next_thread = Scheduler::the().schedule();
        context = &next_thread.unstash_context();

        return context;
    }
}
