#include <hardware/structs/scb.h>
#include <hardware/structs/systick.h>

#include <Kernel/Scheduler.hpp>

extern "C" {
    Kernel::Task *current_task = nullptr;

    void schedule_next_task()
    {
    }

    void isr_systick()
    {
        scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
    }
}

namespace Kernel {

Scheduler::Scheduler()
{
    current_task = new Task { .top_of_stack = nullptr };

    systick_hw->rvr = 10 * 1000;

    systick_hw->csr = 1 << M0PLUS_SYST_CSR_CLKSOURCE_LSB
                    | 1 << M0PLUS_SYST_CSR_TICKINT_LSB
                    | 1 << M0PLUS_SYST_CSR_ENABLE_LSB;
}

}
