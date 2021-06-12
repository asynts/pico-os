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
    // FIXME: Rename 'isr_svcall' to 'syscall_handler'

    // Implemented in Assembly
    extern "C"
    void isr_svcall(u32 syscall, TypeErasedValue arg1, TypeErasedValue arg2, TypeErasedValue arg3);

    // Called from Assembly after saving context in system call handler
    extern "C"
    void syscall_enter(FullRegisterContext *context)
    {
        auto& thread = *Scheduler::the().active();

        // FIXME: We have to be careful with the scheduler now

        // We postpone the processing to unmask interrupts, therefor, we need to block
        // the thread until the system call completes
        thread.m_blocked = true;

        FIXME();
        /*
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
        */
    }

    // Called from Assembly when returning from system call handler. The (previously) active thread is
    // now blocked and we have to hand over to the scheduler
    extern "C"
    void syscall_return_trampoline(FullRegisterContext*)
    {
        FIXME();
    }
}
