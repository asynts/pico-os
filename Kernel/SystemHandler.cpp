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
        thread->m_blocked = true;
        m_waiting_threads.enqueue(move(thread));

        // Notify the SystemHandler thread that is will spawn the system call worker.
        m_thread->wakeup();
    }

    void SystemHandler::handle_next_waiting_thread()
    {
        // FIXME: Disable interrupts.
        RefPtr<Thread> thread = m_waiting_threads.dequeue();

        dbgln("[SystemHandler] Dealing with system call for '{}'", thread->m_name);

        // FIXME
    }

    SystemHandler::SystemHandler()
    {
        m_thread = Thread::construct("Kernel: SystemHandler");
        m_thread->m_privileged = true;
        m_thread->m_blocked = true;
        m_thread->setup_context([&] {
            while (true) {
                bool were_interrupts_enabled = disable_interrupts();
                VERIFY(were_interrupts_enabled);

                // To avoid a lost wakeup problem, we need to make this check with interrupts disabled.
                // When adding multi-core support, we will also need a mutex here.
                VERIFY(not are_interrupts_enabled());
                if (m_waiting_threads.size() == 0) {
                    // If another thread tries to make a system call, it will call 'Thread::wakeup()' which will schedule us again.
                    Thread::active().m_blocked = true;

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

            /*
                // FIXME: We need some way of looping over all blocked threads.
                for (Thread& thread : Scheduler::the().m_blocked_threads) {
                    if (thread.m_requested_system_call) {
                        thread.m_requested_system_call = false;

                        auto new_worker_thread_name = ImmutableString::format("Worker: {} ({})",
                            thread.m_process->m_name,
                            thread.m_process->m_process_id);

                        auto new_worker_thread = Thread::construct(new_worker_thread_name);
                        new_worker_thread->m_privileged = true;

                        new_worker_thread->setup_context([] {
                            // FIXME: Somehow, we need to get hold of the register context that was passed to 'syscall'.
                        });
                    }
                }

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

            */
        });
    }

    extern "C"
    FullRegisterContext& syscall(FullRegisterContext& context)
    {
        RefPtr<Thread> thread = Scheduler::the().get_active_thread();
        thread->stash_context(context);

        SystemHandler::the().notify_worker_thread(move(thread));

        Thread& next_thread = Scheduler::the().schedule();
        return next_thread.unstash_context();
    }
}
