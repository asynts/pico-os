#include <Kernel/Threads/Scheduler.hpp>

#include <hardware/structs/scb.h>
#include <hardware/structs/systick.h>

namespace Kernel
{
    extern "C"
    {
        FullRegisterContext* scheduler_next(FullRegisterContext *context)
        {
            Thread *current = Scheduler::the().active();
            current->stash_context(*context);

            Thread *next = Scheduler::the().schedule();
            context = &next->unstash_context();

            return context;
        }

        void isr_systick()
        {
            if (Scheduler::the().m_enabled)
                scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
        }
    }

    Scheduler::Scheduler()
        : m_default_thread("Default Thread (Core 0)")
    {
        m_default_thread.setup_context([] {
            for (;;) {
                asm volatile ("wfi");
            }
        });

        // FIXME: Figure out a sensitive value, 0x00ffffff is the maximum
        // FIXME: Check that this is correct
        systick_hw->rvr = 0x00f00000;

        systick_hw->csr = 1 << M0PLUS_SYST_CSR_CLKSOURCE_LSB
                        | 1 << M0PLUS_SYST_CSR_TICKINT_LSB
                        | 1 << M0PLUS_SYST_CSR_ENABLE_LSB;
    }

    Thread* Scheduler::schedule()
    {
        if (!m_active_thread->m_die_at_next_opportunity) {
            m_queued_threads.enqueue(m_active_thread);
            m_active_thread = nullptr;
        }

        // FIXME: This algorithm is a bit fishy

        bool all_threads_blocking = true;
        for (size_t i = 0; i < m_queued_threads.size(); ++i) {
            if (!m_queued_threads[i]->m_blocked) {
                all_threads_blocking = false;
                break;
            }
        }

        Thread *next;

        if (all_threads_blocking) {
            next = &m_default_thread;
        } else {
            for (;;) {
                next = m_queued_threads.dequeue();

                if (next->m_blocked) {
                    m_queued_threads.enqueue(next);
                } else {
                    break;
                }
            }
        }

        m_active_thread = next;

        return next;
    }

    void Scheduler::loop()
    {
        // FIXME: This is a hack, we should create a thread and manually switch to it instead.
        //        As result, we have to deal with an additional edge case in Assembly.

        Thread dummy_thread { "Dummy" };
        dummy_thread.m_die_at_next_opportunity = true;
        m_active_thread = &dummy_thread;

        m_enabled = true;

        // The next SysTick interrupt will terminate this context

        for (;;) {
            asm volatile("wfi");
        }
    }
}
