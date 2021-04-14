#include <Kernel/Scheduler.hpp>
#include <Kernel/SystemHandler.hpp>

#include <hardware/structs/scb.h>
#include <hardware/structs/systick.h>

namespace Kernel
{
    extern "C"
    {
        RegisterContext* scheduler_next(RegisterContext *context)
        {
            return Scheduler::the().schedule_next(context);
        }

        void isr_systick()
        {
            if (Scheduler::the().enabled())
                scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
        }
    }

    Scheduler::Scheduler()
    {
        m_enabled = false;

        // FIXME: Figure out a sensitive value, 0x00ffffff is the maximum
        systick_hw->rvr = 0x00f00000;

        systick_hw->csr = 1 << M0PLUS_SYST_CSR_CLKSOURCE_LSB
                        | 1 << M0PLUS_SYST_CSR_TICKINT_LSB
                        | 1 << M0PLUS_SYST_CSR_ENABLE_LSB;
    }

    Thread& Scheduler::create_thread_impl(Thread&& thread, StackWrapper stack, void (*callback)(void*), void *this_)
    {
        dbgln("[Scheduler::create_thread_impl] Creating RegisterContext for thread={}", &thread);

        constexpr u32 xpsr_thumb_mode = 1 << 24;

        RegisterContext context;

        context.xpsr.m_storage = xpsr_thumb_mode;
        context.pc.m_storage = u32(callback);
        context.lr.m_storage = 0;
        context.ip.m_storage = 0;
        context.r3.m_storage = 0;
        context.r2.m_storage = 0;
        context.r1.m_storage = 0;
        context.r0.m_storage = u32(this_);

        context.r4.m_storage = 0;
        context.r5.m_storage = 0;
        context.r6.m_storage = 0;
        context.r7.m_storage = 0;
        context.r8.m_storage = 0;
        context.r9.m_storage = 0;
        context.r10.m_storage = 0;
        context.r11.m_storage = 0;

        stack.align(8);
        u8 *context_data = stack.push(bytes_from(context));

        VERIFY(!thread.m_context.is_valid());
        thread.m_context = reinterpret_cast<RegisterContext*>(context_data);

        return m_threads.enqueue(move(thread));
    }

    void Scheduler::donate_my_remaining_cpu_slice()
    {
        VERIFY(Scheduler::the().enabled());
        scb_hw->icsr = M0PLUS_ICSR_PENDSVSET_BITS;
    }

    void Scheduler::loop()
    {
        Thread thread { __PRETTY_FUNCTION__ };

        // This thread has to be privileged because we want to donate it's CPU time
        thread.m_privileged = true;

        usize stack_size = 0x800;
        u8 *stack_data = new u8[stack_size];
        StackWrapper stack { { stack_data, stack_size } };

        u8 *stack_pointer = stack.align(8);

        VERIFY(!thread.m_context.is_valid());
        m_threads.enqueue_front(move(thread));

        asm volatile("msr psp, %0;"
                     "msr control, %1;"
                     "isb;"
            :
            : "r"(stack_pointer), "r"(0b10));

        dbgln("[Scheduler::loop] Enableling scheduling...");
        m_enabled = true;

        for(;;) {
            donate_my_remaining_cpu_slice();
            asm volatile("wfi");
        }
    }

    RegisterContext* Scheduler::schedule_next(RegisterContext *context)
    {
        Thread thread = m_threads.dequeue();
        VERIFY(!thread.m_context.is_valid());
        thread.m_context = context;
        m_threads.enqueue(move(thread));

        context = m_threads.front().m_context.must();
        m_threads.front().m_context.clear();

        // Note. Writing to CONTROL.SPSEL is ignored by the processor in this context,
        // because we are running in handler mode
        if (m_threads.front().m_privileged) {
            asm volatile(
                "msr control, %0;"
                "isb;"
                :
                : "r"(0b00));
        } else {
            asm volatile(
                "msr control, %0;"
                "isb;"
                :
                : "r"(0b01));
        }

        return context;
    }
}
