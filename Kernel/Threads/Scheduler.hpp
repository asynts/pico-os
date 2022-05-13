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
        Thread* get_active_thread_if_avaliable()
        {
            VERIFY(is_executing_in_handler_mode() || !are_interrupts_enabled());
            return m_active_thread;
        }

        Thread& get_active_thread()
        {
            VERIFY(is_executing_in_handler_mode() || !are_interrupts_enabled());
            VERIFY(m_active_thread != nullptr);
            return *m_active_thread;
        }

        void clear_active_thread()
        {
            VERIFY(is_executing_in_handler_mode() || !are_interrupts_enabled());
            VERIFY(!m_active_thread.is_null());
            m_active_thread.clear();
        }

        Thread& schedule();

        void add_thread(RefPtr<Thread> thread)
        {
            VERIFY(is_executing_in_handler_mode() || !are_interrupts_enabled());
            m_queued_threads.enqueue(thread);
        }

        void dump();

        void loop();
        void trigger();

        bool m_enabled = false;

        // In thread mode, we must disable interrupts to interact with these.
        // For multi-thread support, we should add a mutex here.
        CircularQueue<RefPtr<Thread>, 16> m_queued_threads;
        CircularQueue<RefPtr<Thread>, 16> m_dangling_threads;
        RefPtr<Thread> m_active_thread = nullptr;

    private:
        RefPtr<Thread> m_default_thread;
        RefPtr<Thread> m_fallback_thread;

        friend Singleton<Scheduler>;
        Scheduler(RefPtr<Thread> startup_thread);

        RefPtr<Thread> choose_default_thread();
    };
}
