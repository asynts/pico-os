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
        dbgln("[syscall] entered");

        auto& thread = *Scheduler::the().active();

        thread.m_blocked = true;

        if (context->r0.syscall() == _SC_read) {
            thread.m_running_system_call = SystemCallInfo {
                .m_type = _SC_read,
                .m_thread = &thread,
                .m_data = {
                    .m_read = {
                        .m_fd = context->r1.fd(),
                        .m_buffer = { context->r2.pointer<u8>(), context->r3.value<usize>() },
                    },
                },
            };
        } else if (context->r0.syscall() == _SC_write) {
            thread.m_running_system_call = SystemCallInfo {
                .m_type = _SC_write,
                .m_thread = &thread,
                .m_data = {
                    .m_write = {
                        .m_fd = context->r1.fd(),
                        .m_buffer = { context->r2.pointer<const u8>(), context->r3.value<usize>() },
                    },
                },
            };
        } else {
            FIXME();
        }

        Thread *current = Scheduler::the().active();
        current->stash_context(*context);

        Thread *next = Scheduler::the().schedule();
        context = &next->unstash_context();

        dbgln("[syscall] leaving");

        return context;
    }
}
