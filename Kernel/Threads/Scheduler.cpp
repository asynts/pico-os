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

            if (current != nullptr) {
                VERIFY(!current->m_stashed_context.is_valid());
                current->m_stashed_context = context;
            }

            Thread *next = Scheduler::the().schedule();
            context = next->m_stashed_context.must();
            next->m_stashed_context.clear();

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
        // FIXME: Remember to drop threads if 'm_die_at_next_opportunity' is set

        // FIXME: We only want to choose the default thread if we have no other choice

        // FIXME: Deal with blocked threads

        FIXME();
    }

    void Scheduler::loop()
    {
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
