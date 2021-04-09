#pragma once

#include <Std/Singleton.hpp>
#include <Std/String.hpp>
#include <Std/CircularQueue.hpp>

namespace Kernel
{
    using namespace Std;

    struct Thread {
        struct Stack {
            Stack()
            {
                m_buffer = new u8[0x400];
                m_stack_if_inactive = m_buffer + 0x400;
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

            void push(ReadonlyBytes bytes)
            {
                m_stack_if_inactive = m_stack_if_inactive.must() - bytes.size();
                bytes.copy_to({ m_stack_if_inactive.value(), bytes.size() });
            }

            void push(u32 value)
            {
                push(bytes_from(value));
            }

            template<typename T>
            void push(T *value)
            {
                static_assert(sizeof(T*) == sizeof(u32));
                push(reinterpret_cast<u32>(value));
            }

            void align(u32 boundary)
            {
                u32 stack = reinterpret_cast<u32>(m_stack_if_inactive.must());

                if (stack % boundary != 0)
                    stack -= stack % boundary;

                m_stack_if_inactive = reinterpret_cast<u8*>(stack);
            }
        };

        String m_name;
        Stack m_stack;
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        void create_thread(StringView name, void (*callback)());

        [[noreturn]]
        void loop();

        // Used by PendSV exception handler should return Thread.m_stack.m_current_stack
        u8* schedule_next(u8 *stack);

        bool enabled() const { return m_enabled; }

    private:
        friend Singleton<Scheduler>;
        Scheduler();

        CircularQueue<Thread, 8> m_threads;
        volatile bool m_enabled;
    };
}
