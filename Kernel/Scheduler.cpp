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
        u8 *stack = new u8[0x400] + 0x400;

        const auto push = [&](u32 value) {
            stack = stack - 4;
            *reinterpret_cast<u32*>(stack) = value;
        };
        const auto align = [&](u32 boundary) {
            if (u32(stack) % boundary != 0)
                stack -= u32(stack) % boundary;
        };

        // FIXME: Deal with lambdas

        // FIXME: Add wrapper that deals with threads that return

        constexpr u32 thumb_mask = 1 << 24;

        align(8);

        // Unpacked on exception return
        push(thumb_mask); // XPSR
        push(reinterpret_cast<u32>(callback)); // ReturnAddress
        push(0); // LR (R14)
        push(0); // IP (R12)
        push(0); // R3
        push(0); // R2
        push(0); // R1
        push(0); // R0

        // Unpacked by context switch routine
        push(0); // R4
        push(0); // R5
        push(0); // R6
        push(0); // R7
        push(0); // R8
        push(0); // R9
        push(0); // R10
        push(0); // R11

        // FIXME: Mask PendSV for this operation
        m_threads.enqueue({ name, stack });
    }

    void Scheduler::loop()
    {
        u8 *stack = new u8[0x400] + 0x400;

        const auto align = [&](u32 boundary) {
            if (u32(stack) % boundary != 0)
                stack -= u32(stack) % boundary;
        };

        align(8);

        m_threads.enqueue_front({ __PRETTY_FUNCTION__, {} });

        asm volatile("msr psp, %0;"
                     "msr control, %1;"
                     "isb;"
            :
            : "r"(stack), "r"(0b10));

        m_enabled = true;

        for(;;)
            __wfi();
    }

    u8* Scheduler::schedule_next(u8 *stack)
    {
        Thread thread = m_threads.dequeue();
        thread.m_stack = stack;

        m_threads.enqueue(move(thread));

        stack = m_threads.front().m_stack.must();
        m_threads.front().m_stack.clear();

        return stack;
    }
}
