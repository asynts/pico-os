#include <Kernel/Threads/Thread.hpp>
#include <Kernel/Threads/Scheduler.hpp>
#include <Kernel/Interface/System.hpp>
#include <Kernel/Process.hpp>
#include <Kernel/HandlerMode.hpp>

namespace Kernel
{
    Thread::Thread(String name)
        : m_name(move(name))
    {
        auto& flash_region = m_regions.append({});
        flash_region.rbar.region = 0;
        flash_region.rbar.valid = 0;
        flash_region.rbar.addr = 0x10000000 >> 5;
        flash_region.rasr.enable = 1;
        flash_region.rasr.size = 20;
        flash_region.rasr.srd = 0b00000000;
        flash_region.rasr.attrs_b = 1;
        flash_region.rasr.attrs_c = 1;
        flash_region.rasr.attrs_s = 1;
        flash_region.rasr.attrs_tex = 0b000;
        flash_region.rasr.attrs_ap = 0b111;
        flash_region.rasr.attrs_xn = 0;
    }

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

    void Thread::block()
    {
        if (debug_thread)
            dbgln("[Thread::block] '{}' ({})", m_name, this);
        m_blocked = true;
    }

    void Thread::unblock()
    {
        if (debug_thread)
            dbgln("[Thread::unblock] '{}' ({})", m_name, this);
        m_blocked = false;
    }

    i32 Thread::syscall(u32 syscall, TypeErasedValue arg1, TypeErasedValue arg2, TypeErasedValue arg3)
    {
        switch (syscall) {
        case _SC_read:
            return sys$read(arg1.fd(), arg2.pointer<u8>(), arg3.value<usize>());
        case _SC_write:
            return sys$write(arg1.fd(), arg2.pointer<const u8>(), arg3.value<usize>());
        }

        FIXME();
    }

    i32 Thread::sys$read(i32 fd, u8 *buffer, usize count)
    {
        auto& handle = m_process->get_file_handle(fd);

        auto result = handle.read({ buffer, count });

        if (result.is_error()) {
            return -result.error();
        } else {
            return static_cast<i32>(result.value());
        }
    }

    i32 Thread::sys$write(i32 fd, const u8 *buffer, usize count)
    {
        auto& handle = m_process->get_file_handle(fd);

        auto result = handle.write({ buffer, count });

        if (result.is_error()) {
            return -result.error();
        } else {
            return static_cast<i32>(result.value());
        }
    }
}
