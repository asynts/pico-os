#pragma once

#include <Std/Singleton.hpp>
#include <Std/Vector.hpp>
#include <Std/CircularQueue.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/Threads/Thread.hpp>
#include <Kernel/SystemHandler.hpp>
#include <Kernel/PageAllocator.hpp>

namespace Kernel
{
    constexpr bool debug_scheduler = false;
    constexpr bool scheduler_slow = false;

    class Scheduler : public Singleton<Scheduler> {
    public:
        Thread* active_thread_if_avaliable()
        {
            return m_active_thread;
        }

        Thread& active()
        {
            VERIFY(m_active_thread != nullptr);
            return *m_active_thread;
        }

        Thread& schedule();

        void add_thread(RefPtr<Thread> thread)
        {
            m_queued_threads.enqueue(thread);
        }

        void loop();
        void trigger();

        bool m_enabled = false;

        CircularQueue<RefPtr<Thread>, 16> m_queued_threads;

    private:
        RefPtr<Thread> m_default_thread;

        RefPtr<Thread> m_active_thread = nullptr;

        friend Singleton<Scheduler>;
        Scheduler();
    };
}
