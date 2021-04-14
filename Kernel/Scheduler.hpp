#pragma once

#include <Std/Singleton.hpp>
#include <Std/String.hpp>
#include <Std/CircularQueue.hpp>

#include <Kernel/Process.hpp>
#include <Kernel/SystemHandler.hpp>

namespace Kernel
{
    using namespace Std;

    // FIXME: I need to figure out who should own the stack
    struct StackWrapper {
        explicit StackWrapper(Bytes bytes)
            : m_bytes(bytes)
            , m_top(bytes.data() + bytes.size())
        {
        }

        u8* reserve(usize count)
        {
            ASSERT(m_bytes.data() + count <= m_top);
            return m_top -= count;
        }

        u8* push(ReadonlyBytes bytes)
        {
            u8 *data = reserve(bytes.size());
            bytes.copy_to({ data, bytes.size() });
            return data;
        }

        u8* align(u32 boundary)
        {
            static_assert(sizeof(u8*) == sizeof(u32));
            if (u32(m_top) % boundary != 0)
                reserve(u32(m_top) % boundary);

            return m_top;
        }

    private:
        Bytes m_bytes;
        u8 *m_top;
    };

    class Thread {
    public:
        Thread(StringView name, Optional<Process> process = {}, Optional<RegisterContext*> context = {})
            : m_name(name)
            , m_process(move(process))
            , m_context(context)
        {
        }

        String m_name;
        Optional<Process> m_process;
        Optional<RegisterContext*> m_context;
        bool m_privileged = false;
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        template<typename T, void (T::*Method)()>
        static void wrap_member_function_call_magic(void *object)
        {
            (reinterpret_cast<T*>(object)->*Method)();
        }

        Thread& active_thread()
        {
            return m_threads.front();
        }

        void donate_my_remaining_cpu_slice();

        Thread& create_thread(Thread&& thread)
        {
            VERIFY(thread.m_context.is_valid());
            return m_threads.enqueue(move(thread));
        }

        template<typename Callback>
        Thread& create_thread(Thread&& thread, Callback&& callback)
        {
            auto wrapper = [callback_ = move(callback)]() mutable {
                callback_();
                FIXME();
            };

            VERIFY(!thread.m_context.is_valid());

            usize stack_size = 0x1000;
            u8 *stack_data = new u8[stack_size];
            StackWrapper stack { { stack_data, stack_size } };

            u8 *moved_wrapper = stack.reserve(sizeof(decltype(wrapper)));
            new (moved_wrapper) decltype(wrapper) { move(wrapper) };

            // FIXME: Simplify this somehow

            void (*wrapper_wrapper_function_pointer)(void*) = wrap_member_function_call_magic<decltype(wrapper), &decltype(wrapper)::operator()>;

            return create_thread_impl(move(thread), stack, wrapper_wrapper_function_pointer, moved_wrapper);
        }

        template<typename Callback>
        Thread& create_thread(StringView name, Callback&& callback)
        {
            Thread thread { String::format("Kernel: {}", name) };

            return create_thread(move(thread), move(callback));
        }

        [[noreturn]]
        void loop();

        // Used by PendSV exception handler should return Thread.m_stack.m_current_stack
        RegisterContext* schedule_next(RegisterContext*);

        bool enabled() const { return m_enabled; }

    private:
        friend Singleton<Scheduler>;
        Scheduler();

        Thread& create_thread_impl(Thread&& thread, StackWrapper, void (*callback)(void*), void *this_);

        CircularQueue<Thread, 8> m_threads;
        volatile bool m_enabled;
    };
}
