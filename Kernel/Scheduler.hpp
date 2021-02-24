#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>
#include <Std/Vector.hpp>

namespace Kernel
{
    using namespace Std;

    struct Task {
        Task()
        {
            top_of_stack = new u8[0x1000] + 0x1000;
        }

        template<typename T>
        T* push_onto_stack(T value)
        {
            T *pointer = reinterpret_cast<T*>(top_of_stack -= sizeof(T));
            *pointer = move(value);
            return pointer;
        }

        u8 *top_of_stack;
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        Scheduler();

        void create_task(void (*callback)(void));
        Task* next_task();

        [[noreturn]]
        void loop();

        bool is_enabled() const { return m_enabled; }

    private:
        bool m_enabled = false;
        Vector<Task*> m_tasks;
        usize m_next_task_index = 0;
    };
}
