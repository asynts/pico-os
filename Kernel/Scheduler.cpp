#include <hardware/structs/scb.h>
#include <hardware/structs/systick.h>
#include <hardware/gpio.h>

#include <Kernel/Scheduler.hpp>

extern "C" {
    Kernel::Task *current_task = nullptr;

    // Setting current_task null if it wasn't previously, is undefined behaviour.
    void prepare_next_task()
    {
        current_task = Kernel::Scheduler::the().next_task();
    }

    void isr_systick()
    {
        if (Kernel::Scheduler::the().is_enabled())
            scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
    }

    void isr_hardfault()
    {
        for(;;) {
            gpio_init(25);
            gpio_set_dir(25, true);
            gpio_put(25, true);

            __wfi();
        }
    }
}

namespace Kernel {

Scheduler::Scheduler()
{
    systick_hw->rvr = 10 * 1000;

    systick_hw->csr = 1 << M0PLUS_SYST_CSR_CLKSOURCE_LSB
                    | 1 << M0PLUS_SYST_CSR_TICKINT_LSB
                    | 1 << M0PLUS_SYST_CSR_ENABLE_LSB;
}

void Scheduler::create_task(void (*callback)(void))
{
    Task *task = new Task;

    task->push_onto_stack<u32>(0); // r7
    task->push_onto_stack<u32>(0); // r6
    task->push_onto_stack<u32>(0); // r5
    task->push_onto_stack<u32>(0); // r4
    task->push_onto_stack<void(*)()>(callback); // lr
    task->push_onto_stack<u32>(0); // r11
    task->push_onto_stack<u32>(0); // r10
    task->push_onto_stack<u32>(0); // r9
    task->push_onto_stack<u32>(0); // r8

    m_tasks.append(task);
}

Task* Scheduler::next_task()
{
    if (m_tasks.size() == 0)
        return nullptr;

    return m_tasks[m_next_task_index++ % m_tasks.size()];
}

void Scheduler::loop()
{
    m_enabled = true;

    for(;;)
        __wfi();
}

}
