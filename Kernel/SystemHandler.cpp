#include <Std/Forward.hpp>

#include <Kernel/Interface/System.hpp>
#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/FileSystem/MemoryFileSystem.hpp>
#include <Kernel/Process.hpp>
#include <Kernel/FileSystem/FileSystem.hpp>
#include <Kernel/Interface/Types.hpp>
#include <Kernel/GlobalMemoryAllocator.hpp>
#include <Kernel/Threads/Scheduler.hpp>
#include <Kernel/Threads/Thread.hpp>

namespace Kernel
{
    void SystemHandler::notify_worker_thread(RefPtr<Thread> thread)
    {
        thread->set_masked_from_scheduler(true);

        VERIFY(is_executing_in_handler_mode());
        m_waiting_threads.enqueue(move(thread));

        // Notify the SystemHandler thread that is will spawn the system call worker.
        m_thread->wakeup();
    }

    void SystemHandler::handle_next_waiting_thread()
    {
        RefPtr<Thread> thread;
        {
            MaskedInterruptGuard interrupt_guard;
            thread = m_waiting_threads.dequeue();
        }

        dbgln("[SystemHandler] Dealing with system call for '{}'", thread->m_name);

        auto new_worker_thread_name = ImmutableString::format("Worker: '{}' (PID {})",
            thread->m_process->m_name,
            thread->m_process->m_process_id);

        auto new_worker_thread = Thread::construct(new_worker_thread_name);
        new_worker_thread->m_privileged = true;

        new_worker_thread->setup_context([thread = move(thread)] () mutable {
            // We can not consume the register context here, since it is needed to continue execution.
            FullRegisterContext& context = *thread->m_stashed_context.must();

            i32 return_value = thread->syscall(context.r0.syscall(), context.r1, context.r2, context.r3);

            if (context.r0.syscall() == _SC_exit) {
                VERIFY(thread->m_masked_from_scheduler);
            }

            // System calls return values by magically tweaking the value of the 'r0' register when returning.
            context.r0.m_storage = bit_cast<u32>(return_value);

            thread->set_masked_from_scheduler(false);

            {
                MaskedInterruptGuard scheduler_guard;
                Scheduler::the().add_thread(move(thread));
            }
        });

        {
            MaskedInterruptGuard scheduler_guard;
            Scheduler::the().add_thread(move(new_worker_thread));
        }
    }

    SystemHandler::SystemHandler()
    {
        m_thread = Thread::construct("Kernel: SystemHandler");
        m_thread->m_privileged = true;
        m_thread->set_masked_from_scheduler(true);
        m_thread->setup_context([&] {
            while (true) {
                bool were_interrupts_enabled = disable_interrupts();
                VERIFY(were_interrupts_enabled);

                // To avoid a lost wakeup problem, we need to make this check with interrupts disabled.
                // When adding multi-core support, we will also need a mutex here.
                VERIFY(not are_interrupts_enabled());
                if (m_waiting_threads.size() == 0) {
                    // If another thread tries to make a system call, it will call 'Thread::wakeup()' which will schedule us again.
                    {
                        MaskedInterruptGuard interrupt_guard;
                        Scheduler::the().get_active_thread().set_masked_from_scheduler(true);
                    }

                    // Since this thread is blocking, we will not be requeued until another system call occurs.
                    Scheduler::the().trigger();

                    // Since we triggered the scheduler, the moment the interrupts are turned on, a context switch should occur.
                    restore_interrupts(were_interrupts_enabled);
                    VERIFY(are_interrupts_enabled());

                    continue;
                }

                restore_interrupts(were_interrupts_enabled);
                VERIFY(are_interrupts_enabled());

                // Now, we know that another thread is in the list and we can take it.
                handle_next_waiting_thread();
            }
        });
    }

    extern "C"
    FullRegisterContext& syscall(FullRegisterContext& context)
    {
        RefPtr<Thread> thread = Scheduler::the().get_active_thread();

        thread->stash_context(context);
        Scheduler::the().clear_active_thread();

        SystemHandler::the().notify_worker_thread(move(thread));

        // Since the active thread has been cleared, we will not save any context here.
        Thread& next_thread = Scheduler::the().schedule();
        return next_thread.unstash_context();
    }
}
