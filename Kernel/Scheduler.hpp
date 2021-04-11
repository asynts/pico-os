#pragma once

#include <Std/Singleton.hpp>
#include <Std/String.hpp>
#include <Std/CircularQueue.hpp>

namespace Kernel
{
    using namespace Std;

    struct Thread {
        explicit Thread(StringView name)
            : m_name(name)
        {
        }

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
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        template<typename T, void (T::*Method)()>
        static void wrap_member_function_call_magic(void *object)
        {
            (reinterpret_cast<T*>(object)->*Method)();
        }

        template<typename Callback>
        void create_thread(StringView name, Callback&& callback)
        {
            Thread thread { name };

            auto wrapper = [callback_ = move(callback)]() mutable {
                callback_();
                FIXME();
            };

            u8 *moved_wrapper = thread.m_stack.reserve(sizeof(decltype(wrapper)));
            new (moved_wrapper) decltype(wrapper) { move(wrapper) };

            // FIXME: Simplify this somehow

            void (*wrapper_wrapper_function_pointer)(void*) = wrap_member_function_call_magic<decltype(wrapper), &decltype(wrapper)::operator()>;

            create_thread_impl(move(thread), wrapper_wrapper_function_pointer, moved_wrapper);
        }

        [[noreturn]]
        void loop();

        // Used by PendSV exception handler should return Thread.m_stack.m_current_stack
        u8* schedule_next(u8 *stack);

        bool enabled() const { return m_enabled; }

    private:
        friend Singleton<Scheduler>;
        Scheduler();

        void create_thread_impl(Thread&& thread, void (*callback)(void*), u8 *this_);

        CircularQueue<Thread, 8> m_threads;
        volatile bool m_enabled;
    };
}
