#include <hardware/structs/scb.h>

#include <Kernel/Scheduler.hpp>

extern "C" {
    Kernel::Task *current_task = nullptr;

    void schedule_next_task()
    {
        panic("Yay! schedule_next_task got called!");
    }

    void isr_systick()
    {
        scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
    }
}

namespace Kernel {

Scheduler::Scheduler()
{
}

}
