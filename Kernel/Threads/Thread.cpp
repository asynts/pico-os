#include <Kernel/Threads/Thread.hpp>

namespace Kernel
{
    void Thread::setup_context_impl(StackWrapper stack_wrapper, void (*callback)(void*), void* argument)
    {
        constexpr u32 xpsr_thumb_mode = 1 << 24;

        FullRegisterContext context;

        context.xpsr.m_storage = xpsr_thumb_mode;
        context.pc.m_storage = u32(callback);
        context.lr.m_storage = 0;
        context.ip.m_storage = 0;
        context.r3.m_storage = 0;
        context.r2.m_storage = 0;
        context.r1.m_storage = 0;
        context.r0.m_storage = u32(argument);

        context.r4.m_storage = 0;
        context.r5.m_storage = 0;
        context.r6.m_storage = 0;
        context.r7.m_storage = 0;
        context.r8.m_storage = 0;
        context.r9.m_storage = 0;
        context.r10.m_storage = 0;
        context.r11.m_storage = 0;

        stack_wrapper.align(8);

        VERIFY(!m_stashed_context.is_valid());
        m_stashed_context = stack_wrapper.push_value(context);
    }
}
