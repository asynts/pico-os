#include <Kernel/Threads/Scheduler.hpp>
#include <Kernel/Loader.hpp>
#include <Kernel/HandlerMode.hpp>

#include <hardware/structs/scb.h>
#include <hardware/structs/systick.h>

namespace Kernel
{
    extern "C"
    {
        FullRegisterContext& scheduler_next(FullRegisterContext& context)
        {
            if (debug_scheduler)
                dbgln("[scheduler_next]");

            Thread& active_thread = Scheduler::the().get_active_thread();
            active_thread.stash_context(context);

            Thread& next_thread = Scheduler::the().schedule();
            return next_thread.unstash_context();
        }

        void isr_systick()
        {
            if (Scheduler::the().m_enabled)
                scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
        }
    }

    Scheduler::Scheduler()
    {
        if (scheduler_slow)
            systick_hw->rvr = 0x00f00000;
        else
            systick_hw->rvr = 0x000f0000;

        systick_hw->csr = 1 << M0PLUS_SYST_CSR_CLKSOURCE_LSB
                        | 1 << M0PLUS_SYST_CSR_TICKINT_LSB
                        | 1 << M0PLUS_SYST_CSR_ENABLE_LSB;
    }

    Thread& Scheduler::schedule()
    {
        VERIFY(is_executing_in_handler_mode());

        // First, we need to save the previous active thread somehow.
        if (m_active_thread.is_null()) {
            // There are situations where we do not have an active thread.
            // This can happen when we are in a system call for example.
        } else {
            if (m_active_thread->m_masked_from_scheduler) {
                // We are not allowed to drop the last reference here, because we are in handler mode.
                // If this is the last reference, then we need to clean it up elsewhere.
                if (m_active_thread->refcount() >= 2) {
                    m_active_thread.clear();
                } else {
                    m_dangling_threads.enqueue(move(m_active_thread));
                }
            } else if (m_active_thread->m_is_default_thread) {
                // The default thread should not be queued.
                VERIFY(m_active_thread->refcount() >= 2);
                m_active_thread.clear();
            } else {
                // Schedule this thread again at a later point.
                m_queued_threads.enqueue(move(m_active_thread));
            }
        }

        // Next, we need to choose a new thread to schedule.
        VERIFY(m_active_thread.is_null());
        if (m_dangling_threads.size() >= 1) {
            // We need to drop the reference to a dangling thread.
            m_active_thread = m_default_thread;
        } else if (m_queued_threads.size() == 0) {
            // We have no normal threads that should be scheduled.
            m_active_thread = m_default_thread;
        } else {
            m_active_thread = m_queued_threads.dequeue();
        }

        // Setup control register for privileged/unprivileged execution.
        if (m_active_thread->m_privileged) {
            asm volatile("msr control, %0;"
                         "isb;"
                :
                : "r"(0b10));
        } else {
            asm volatile("msr control, %0;"
                         "isb;"
                :
                : "r"(0b11));
        }

        // Setup the memory protection unit.
        // Since we are in an interrupt handler, this will only apply after we return.
        setup_mpu(m_active_thread->m_regions);

        return m_active_thread.must();
    }

    void Scheduler::trigger()
    {
        VERIFY(m_enabled);
        VERIFY(is_executing_in_thread_mode());

        scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
    }

    void Scheduler::loop()
    {
        m_default_thread = Thread::construct("Default Thread (Core 0)");
        m_default_thread->m_privileged = true;
        m_default_thread->m_is_default_thread = true;

        m_default_thread->setup_context([&] {
            for (;;) {
                dbgln("[Scheduler] Running default thread. (refcount={})", m_default_thread->refcount());

                bool were_enabled = disable_interrupts();
                VERIFY(were_enabled);

                if (m_dangling_threads.size() == 0) {
                    VERIFY(were_enabled);
                    restore_interrupts(were_enabled);

                    // There is nothing sensible we can do, give the scheduler another opportunity.
                    Scheduler::the().trigger();
                    continue;
                }

                RefPtr<Thread> thread = m_dangling_threads.dequeue();

                // At this point, we no longer need to synchronize, the cleanup can happen in parallel.
                restore_interrupts(were_enabled);
                VERIFY(are_interrupts_enabled());

                dbgln("[Scheduler] We are about to kill thread '{}' (refcount={})", thread->m_name, thread->refcount());

                // Remove the last reference to this thread and thus kill it.
                VERIFY(thread->refcount() == 1);
                thread.clear();
            }
        });

        // This is a special thread that should die immediately.
        // We simply need some way of entering the scheduler.
        auto dummy_thread = Thread::construct("Dummy");
        dummy_thread->m_masked_from_scheduler = true;

        dummy_thread->setup_context([] {
            Scheduler::the().m_enabled = true;
            Scheduler::the().trigger();
            VERIFY_NOT_REACHED();
        });

        m_active_thread = dummy_thread;

        FullRegisterContext& context = dummy_thread->unstash_context();

        u32 control = 0b10;
        asm volatile("msr control, %0;"
                     "isb;"
                     "mov r0, %1;"
                     "blx restore_context_from_thread_mode;"
            :
            : "r"(control), "r"(&context)
            : "r0");

        VERIFY_NOT_REACHED();
    }

    void Scheduler::dump()
    {
        dbgln("[Scheduler] m_queued_threads:");
        for (size_t i = 0; i < m_queued_threads.size(); ++i) {
            Thread& thread = *m_queued_threads[i];
            dbgln("  {} @{}", thread.m_name, &thread);
        }

        dbgln("[Scheduler] m_danging_threads:");
        for (size_t i = 0; i < m_dangling_threads.size(); ++i) {
            Thread& thread = *m_dangling_threads[i];
            dbgln("  {} @{}", thread.m_name, &thread);
        }

        dbgln("[Scheduler] m_default_thread:");
        {
            Thread& thread = m_default_thread.must();
            dbgln("  {} @{}", thread.m_name, &thread);
        }
    }
}
