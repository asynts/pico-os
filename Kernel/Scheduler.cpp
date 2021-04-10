#include <Kernel/Scheduler.hpp>

#include <hardware/structs/scb.h>
#include <hardware/structs/systick.h>

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
                    skip_cycles = 5;
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

    void Scheduler::create_thread_impl(Thread&& thread, void (*callback)(void*), u8 *this_)
    {
        constexpr u32 xpsr_thumb_mode = 1 << 24;

        thread.m_stack.align(8);

        // Unpacked on exception return
        thread.m_stack.push(xpsr_thumb_mode); // XPSR
        thread.m_stack.push(callback); // ReturnAddress
        thread.m_stack.push(0); // LR (R14)
        thread.m_stack.push(0); // IP (R12)
        thread.m_stack.push(0); // R3
        thread.m_stack.push(0); // R2
        thread.m_stack.push(0); // R1
        thread.m_stack.push(this_); // R0

        // Unpacked by context switch routine
        thread.m_stack.push(0); // R4
        thread.m_stack.push(0); // R5
        thread.m_stack.push(0); // R6
        thread.m_stack.push(0); // R7
        thread.m_stack.push(0); // R8
        thread.m_stack.push(0); // R9
        thread.m_stack.push(0); // R10
        thread.m_stack.push(0); // R11

        // FIXME: Mask PendSV for this operation
        m_threads.enqueue(move(thread));
    }

    void Scheduler::loop()
    {
        Thread thread { __PRETTY_FUNCTION__ };

        thread.m_stack.align(8);

        u8 *stack_pointer = thread.m_stack.m_stack_if_inactive.must();
        thread.m_stack.m_stack_if_inactive.clear();

        m_threads.enqueue_front(move(thread));

        asm volatile("msr psp, %0;"
                     "msr control, %1;"
                     "isb;"
            :
            : "r"(stack_pointer), "r"(0b10));

        m_enabled = true;

        for(;;)
            asm volatile("wfi");
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
