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

    class Scheduler : public Singleton<Scheduler> {
    public:
        Thread* active()
        {
            return m_active_thread;
        }

        Thread* schedule();

        void add_thread(Thread& thread)
        {
            m_queued_threads.enqueue(&thread);
        }

        void loop();
        void trigger();

        bool m_enabled = false;

        Vector<Thread*> m_blocked_threads;

    private:
        Thread m_default_thread;

        Thread *m_active_thread = nullptr;
        CircularQueue<Thread*, 16> m_queued_threads;

        friend Singleton<Scheduler>;
        Scheduler();
    };

    class SchedulerGuard {
    public:
        SchedulerGuard()
        {
            m_was_enabled = exchange(Scheduler::the().m_enabled, false);
        }
        SchedulerGuard(const SchedulerGuard&) = delete;
        SchedulerGuard(SchedulerGuard&& other)
        {
            m_was_enabled = exchange(other.m_was_enabled, {});
        }
        ~SchedulerGuard()
        {
            if (m_was_enabled.is_valid()) {
                VERIFY(!Scheduler::the().m_enabled);
                Scheduler::the().m_enabled = m_was_enabled.value();
            }
        }

    private:
        Optional<bool> m_was_enabled;
    };
}
