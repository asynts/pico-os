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
    extern "C"
    FullRegisterContext* syscall(FullRegisterContext *context)
    {
        dbgln("[syscall] syscall={}", context->r0.syscall());

        auto& thread = *Scheduler::the().active();

        thread.block();
        thread.stash_context(*context);

        // FIXME: Memory leak
        Thread& worker_thread = *new Thread { String::format("Worker: '{}' ({}): syscall={}", thread.m_name, &thread, context->r0.syscall()) };
        worker_thread.m_privileged = true;
        worker_thread.setup_context([&thread, context] {
            TypeErasedValue return_value = thread.syscall(context->r0.syscall(), context->r1, context->r2, context->r3);
            thread.m_stashed_context.must()->r0 = return_value;

            thread.unblock();
        });
        Scheduler::the().add_thread(worker_thread);

        Thread& next = *Scheduler::the().schedule();
        context = &next.unstash_context();

        return context;
    }
}
