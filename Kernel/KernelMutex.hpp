#pragma once

#include <Std/CircularQueue.hpp>
#include <Std/RefPtr.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/Threads/Scheduler.hpp>

namespace Kernel
{
    // FIXME: Must not be accessed in handler mode

    // FIXME: Public interface, enforce lock guards

    // FIXME: Syncronize lock and unlock themselves

    // FIXME: Fix semantics of Thread::block

    class KernelMutex
    {
    public:
        ~KernelMutex()
        {
            VERIFY(m_waiting_threads.size() == 0);
        }

        void lock()
        {
            // We must not hold a strong reference here, otherwise, this thread could not be
            // terminated without being rescheduled. Not that this should happen, but...
            Thread& active_thread = Scheduler::the().active();

            if (m_holding_thread.is_null()) {
                m_holding_thread = active_thread;
            } else {
                m_waiting_threads.enqueue(active_thread)
                active_thread.block();
            }
        }

        void unlock()
        {
            m_holding_thread.clear();

            if (m_waiting_threads.size() > 0) {
                RefPtr<Thread> next_thread = m_waiting_threads.dequeue();

                FIXME_ASSERT(m_holding_thread.is_null());
                m_holding_thread = next_thread;

                m_holding_thread->unblock();
            }
        }

    private:
        RefPtr<Thread> m_holding_thread;
        CircularQueue<RefPtr<Thread>, 16> m_waiting_threads;
    };
}
