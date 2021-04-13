#pragma once

#include <Std/Singleton.hpp>
#include <Std/String.hpp>
#include <Std/CircularQueue.hpp>

#include <Kernel/Process.hpp>
#include <Kernel/SystemHandler.hpp>

namespace Kernel
{
    using namespace Std;

    struct Thread {
        explicit Thread(StringView name, Optional<Process> process = {})
            : m_name(name)
            , m_process(move(process))
        {
        }

        // FIXME: We somehow need to store a RegisterContext, but the stack abstraction
        //        is useful too
        struct Stack {
            Stack()
            {
                m_buffer = new u8[0x1000];
                m_stack_if_inactive = m_buffer + 0x1000;
            }
            ~Stack()
            {
                delete[] m_buffer;
            }
            Stack(Stack&& other)
            {
                m_buffer = nullptr;
                *this = move(other);
            }

            Stack& operator=(Stack&& other)
            {
                delete[] m_buffer;

                m_buffer = exchange(other.m_buffer, nullptr);
                m_stack_if_inactive = exchange(other.m_stack_if_inactive, {});

                return *this;
            }

            Stack(const Stack&) = delete;
            Stack& operator=(const Stack&) = delete;

            u8 *m_buffer;

            Optional<u8*> m_stack_if_inactive;

            u8* push(ReadonlyBytes bytes)
            {
                m_stack_if_inactive = m_stack_if_inactive.must() - bytes.size();
                bytes.copy_to({ m_stack_if_inactive.value(), bytes.size() });
                return m_stack_if_inactive.value();
            }

            u8* push(u32 value)
            {
                return push(bytes_from(value));
            }

            u8* reserve(usize count)
            {
                m_stack_if_inactive = m_stack_if_inactive.must() - count;
                return m_stack_if_inactive.value();
            }

            template<typename T>
            u8* push(T *value)
            {
                static_assert(sizeof(T*) == sizeof(u32));
                return push(reinterpret_cast<u32>(value));
            }

            u8* align(u32 boundary)
            {
                u32 stack = reinterpret_cast<u32>(m_stack_if_inactive.must());

                if (stack % boundary != 0)
                    stack -= stack % boundary;

                m_stack_if_inactive = reinterpret_cast<u8*>(stack);
                return m_stack_if_inactive.value();
            }
        };

        String m_name;
        Stack m_stack;
        Optional<Process> m_process;
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
            return m_threads.enqueue(move(thread));
        }

        template<typename Callback>
        Thread& create_thread(Thread&& thread, Callback&& callback)
        {
            auto wrapper = [callback_ = move(callback)]() mutable {
                callback_();
                FIXME();
            };

            u8 *moved_wrapper = thread.m_stack.reserve(sizeof(decltype(wrapper)));
            new (moved_wrapper) decltype(wrapper) { move(wrapper) };

            // FIXME: Simplify this somehow

            void (*wrapper_wrapper_function_pointer)(void*) = wrap_member_function_call_magic<decltype(wrapper), &decltype(wrapper)::operator()>;

            return create_thread_impl(move(thread), wrapper_wrapper_function_pointer, moved_wrapper);
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

        Thread& create_thread_impl(Thread&& thread, void (*callback)(void*), u8 *this_);

        CircularQueue<Thread, 8> m_threads;
        volatile bool m_enabled;
    };
}
