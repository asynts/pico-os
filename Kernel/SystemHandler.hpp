#pragma once

#include <Std/Forward.hpp>
#include <Std/Singleton.hpp>
#include <Std/CircularQueue.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/Result.hpp>

namespace Kernel
{
    constexpr bool debug_system_handler = false;

    class SystemHandler : public Singleton<SystemHandler> {
    public:
        void notify_worker_thread(RefPtr<Thread> thread);

    private:
        RefPtr<Thread> m_thread;

        // In thread mode, we must disable interrupts to interact with this.
        // For multi-core support, we would need some sort of mutex here.
        CircularQueue<RefPtr<Thread>, 16> m_waiting_threads;

        friend Singleton<SystemHandler>;
        SystemHandler();

        void handle_next_waiting_thread();
    };

    // FIXME: Most of this stuff should go to different places

    struct TypeErasedValue {
        template<typename T>
        T value() { return bit_cast<T>(m_storage); }

        template<typename T>
        T* pointer() { return bit_cast<T*>(m_storage); }

        u32 syscall() { return value<u32>(); }
        const char* cstring() { return pointer<const char>(); }
        i32 fd() { return value<i32>(); }
        i32 pid() { return value<i32>(); }
        usize size() { return value<usize>(); }

        u32 m_storage;
    };

    struct ExtendedSystemCallArguments {
        TypeErasedValue arg3;
        TypeErasedValue arg4;
        TypeErasedValue arg5;
        TypeErasedValue arg6;
    };

    struct ExceptionRegisterContext {
        TypeErasedValue r0;
        TypeErasedValue r1;
        TypeErasedValue r2;
        TypeErasedValue r3;
        TypeErasedValue ip;
        TypeErasedValue lr;
        TypeErasedValue pc;
        TypeErasedValue xpsr;
    };

    struct FullRegisterContext {
        TypeErasedValue r11;
        TypeErasedValue r10;
        TypeErasedValue r9;
        TypeErasedValue r8;
        TypeErasedValue r7;
        TypeErasedValue r6;
        TypeErasedValue r5;
        TypeErasedValue r4;

        TypeErasedValue r0;
        TypeErasedValue r1;
        TypeErasedValue r2;
        TypeErasedValue r3;
        TypeErasedValue ip;
        TypeErasedValue lr;
        TypeErasedValue pc;
        TypeErasedValue xpsr;
    };
}

template<>
struct Std::Formatter<Kernel::ExceptionRegisterContext> {
    static void format(StringBuilder& builder, const Kernel::ExceptionRegisterContext& context)
    {
        builder.appendf("r0={} r1={} r2={}   r3={}\n", context.r0.m_storage, context.r1.m_storage, context.r2.m_storage, context.r3.m_storage);
        builder.appendf("ip={} lr={} pc={} xpsr={}\n", context.ip.m_storage, context.lr.m_storage, context.pc.m_storage, context.xpsr.m_storage);
    }
};

template<>
struct Std::Formatter<Kernel::FullRegisterContext> {
    static void format(StringBuilder& builder, const Kernel::FullRegisterContext& context)
    {
        builder.appendf("r0={} r1={}  r2={}   r3={}\n", context.r1.m_storage, context.r2.m_storage, context.r3.m_storage, context.r4.m_storage);
        builder.appendf("ip={} lr={}  pc={} xpsr={}\n", context.ip.m_storage, context.lr.m_storage, context.pc.m_storage, context.xpsr.m_storage);
        builder.appendf("r4={} r5={}  r6={}   r7={}\n", context.r4.m_storage, context.r5.m_storage, context.r6.m_storage, context.r7.m_storage);
        builder.appendf("r8={} sb={} r10={}  r11={}\n", context.r8.m_storage, context.r9.m_storage, context.r10.m_storage, context.r11.m_storage);
    }
};
