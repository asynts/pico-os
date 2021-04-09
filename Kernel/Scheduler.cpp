#include <Kernel/Scheduler.hpp>

#include <hardware/structs/scb.h>
#include <hardware/structs/systick.h>
#include <pico/sync.h>

namespace Kernel
{
    extern "C"
    {
        u8* scheduler_next(u8 *stack)
        {
            return Scheduler::the().schedule_next(stack);
        }

        static int skip_cycles = 0;

        void isr_systick()
        {
            if (Scheduler::the().enabled()) {
                if (skip_cycles == 0) {
                    skip_cycles = 20;
                    scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
                } else {
                    skip_cycles--;
                }
            }
        }
    }

    Scheduler::Scheduler()
    {
        m_enabled = false;

        systick_hw->rvr = 0x00ffffff;

        systick_hw->csr = 1 << M0PLUS_SYST_CSR_CLKSOURCE_LSB
                        | 1 << M0PLUS_SYST_CSR_TICKINT_LSB
                        | 1 << M0PLUS_SYST_CSR_ENABLE_LSB;
    }

    void Scheduler::create_thread(StringView name, void (*callback)())
    {
        Thread::Stack stack;

        constexpr u32 xpsr_thumb_mode = 1 << 24;

        stack.align(8);

        // Unpacked on exception return
        stack.push(xpsr_thumb_mode); // XPSR
        stack.push(callback); // ReturnAddress
        stack.push(0); // LR (R14)
        stack.push(0); // IP (R12)
        stack.push(0); // R3
        stack.push(0); // R2
        stack.push(0); // R1
        stack.push(0); // R0

        // Unpacked by context switch routine
        stack.push(0); // R4
        stack.push(0); // R5
        stack.push(0); // R6
        stack.push(0); // R7
        stack.push(0); // R8
        stack.push(0); // R9
        stack.push(0); // R10
        stack.push(0); // R11

        // FIXME: Mask PendSV for this operation
        m_threads.enqueue({ name, move(stack) });
    }

    void Scheduler::loop()
    {
        Thread::Stack stack;

        stack.align(8);

        u8 *stack_pointer = stack.m_stack_if_inactive.must();
        stack.m_stack_if_inactive.clear();

        m_threads.enqueue_front({ __PRETTY_FUNCTION__, move(stack) });

        asm volatile("msr psp, %0;"
                     "msr control, %1;"
                     "isb;"
            :
            : "r"(stack_pointer), "r"(0b10));

        m_enabled = true;

        for(;;)
            __wfi();
    }

    u8* Scheduler::schedule_next(u8 *stack)
    {
        Thread thread = m_threads.dequeue();
        ASSERT(!thread.m_stack.m_stack_if_inactive.is_valid());
        thread.m_stack.m_stack_if_inactive = stack;

        m_threads.enqueue(move(thread));

        stack = m_threads.front().m_stack.m_stack_if_inactive.must();
        m_threads.front().m_stack.m_stack_if_inactive.clear();

        return stack;
    }
}
