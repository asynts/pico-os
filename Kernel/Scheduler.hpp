#pragma once

#include <Std/Singleton.hpp>
#include <Std/String.hpp>
#include <Std/CircularQueue.hpp>

namespace Kernel
{
    using namespace Std;

    struct Thread {
        String m_name;
        Optional<u8*> m_stack;
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        void create_thread(StringView name, void (*callback)());

        [[noreturn]]
        void loop();

        u8* schedule_next(u8 *stack);

        bool enabled() const { return m_enabled; }

    private:
        friend Singleton<Scheduler>;
        Scheduler();

        CircularQueue<Thread, 8> m_threads;
        volatile bool m_enabled;
    };
}
