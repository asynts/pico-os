#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>
#include <Std/Vector.hpp>

namespace Kernel
{
    using namespace Std;

    struct Task {
    public:
        explicit Task(u8 *stack)
            : m_stack(stack)
        {
        }
        Task()
        {
            m_stack = new u8[0x1000] + 0x1000;
            align_stack_to(8);
        }

        template<typename T>
        u32* push_onto_stack(T value)
        {
            return push_onto_stack<u32>(u32(value));
        }

        void align_stack_to(usize align)
        {
            m_stack -= reinterpret_cast<u32>(m_stack) % align;
        }

        u8* stack() { return m_stack; }
        void set_stack(u8 *stack) { m_stack = stack; }

    private:
        u8 *m_stack;
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        Scheduler();

        Task* create_task(void (*callback)(void));

        void loop();
        u8* prepare_next_task(u8 *stack);

    private:
        Vector<Task*> m_tasks;
        usize m_current_task_index = 0;
    };

    template<>
    inline u32* Task::push_onto_stack(u32 value)
    {
        u32 *pointer = reinterpret_cast<u32*>(m_stack -= sizeof(u32));
        *pointer = value;
        return pointer;
    }
}
