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

        // FIXME:   This is a bit yanky
        //          This could be addressed by giving Scheduler a proper constructor and put Scheduler::loop in there

        void lock()
        {
            if (!m_enabled)
                return;

            VERIFY(Kernel::is_executing_in_thread_mode());

            // We only have to lock if the scheduler is initialized and if threads are already being scheduled.

            if (Scheduler::is_initialized()) {
                MaskedInterruptGuard interrupt_guard;

                // We must not hold a strong reference here, otherwise, this thread could not be
                // terminated without being rescheduled. Not that this should happen, but...
                Thread *active_thread = Scheduler::the().get_active_thread_if_avaliable();

                if (active_thread != nullptr) {
                    if (m_holding_thread.is_null()) {
                        m_holding_thread = *active_thread;
                    } else {
                        active_thread->set_masked_from_scheduler(true);
                        m_waiting_threads.enqueue(*active_thread);
                        Scheduler::the().trigger();
                    }
                }
            } else {
                // Since the Scheduler is not initialized, we do not have to deal with locking
            }
        }

        void unlock()
        {
            if (!m_enabled)
                return;

            VERIFY(Kernel::is_executing_in_thread_mode());

            if (Scheduler::is_initialized()) {
                MaskedInterruptGuard interrupt_guard;

                m_holding_thread.clear();

                if (m_waiting_threads.size() > 0) {
                    RefPtr<Thread> next_thread = m_waiting_threads.dequeue();

                    FIXME_ASSERT(m_holding_thread.is_null());
                    m_holding_thread = next_thread;

                    m_holding_thread->wakeup();
                }
            } else {
                // Since the Scheduler is not initialized, we do not have to deal with locking

                VERIFY(m_holding_thread.is_null());
            }
        }

        void set_enabled(bool enabled)
        {
            m_enabled = enabled;
        }

        bool is_locked()
        {
            MaskedInterruptGuard interrupt_guard;
            return !m_holding_thread.is_null();
        }

    private:
        // During the boot procedure it's not always possible to aquire a mutex.
        // Since the scheduler isn't running at that point, we can safely access the resource without a lock.
        volatile bool m_enabled = true;

        RefPtr<Thread> m_holding_thread;
        CircularQueue<RefPtr<Thread>, 16> m_waiting_threads;
    };

    extern KernelMutex dbgln_mutex;
    extern KernelMutex malloc_mutex;
    extern KernelMutex page_allocator_mutex;
}
