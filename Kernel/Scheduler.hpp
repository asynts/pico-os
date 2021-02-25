#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>
#include <Std/Vector.hpp>

namespace Kernel
{
    using namespace Std;

    struct Task {
    public:
        Task()
        {
            m_stack = new u8[0x1000] + 0x1000;
        }

        template<typename T>
        T* push_onto_stack(T value)
        {
            T *pointer = reinterpret_cast<T*>(m_stack -= sizeof(T));
            *pointer = move(value);
            return pointer;
        }

        u8* stack() { return m_stack; }

    private:
        u8 *m_stack;
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        Scheduler();

        void create_task(void (*callback)(void));

        void loop();
        u8* prepare_next_task(u8 *stack);

    private:
        Vector<Task*> m_tasks;
        usize m_next_task_index = 0;
    };
}
