#include <Kernel/Synchronization/SoftwareMutex.hpp>
#include <Kernel/Synchronization/MaskedInterruptGuard.hpp>
#include <Kernel/Threads/Scheduler.hpp>

namespace Kernel
{
    void SoftwareMutex::initialize(HardwareSpinLock& lock)
    {
        m_lock = &lock;
    }

    void SoftwareMutex::lock()
    {
        if (!Scheduler::is_initialized())
            return;

        // Passive lock: must be called with interrupts enabled (logic requirement),
        // but we assume thread context.
        VERIFY(is_executing_in_thread_mode());

        MaskedInterruptGuard interrupt_guard;
        ASSERT(m_lock);

        m_lock->lock();

        if (m_owner.is_null()) {
            m_owner = Scheduler::the().get_active_thread();
            m_lock->unlock();
        } else {
            auto& thread = Scheduler::the().get_active_thread();
            thread.set_masked_from_scheduler(true);
            m_waiting_threads.enqueue(thread);

            m_lock->unlock();

            Scheduler::the().trigger(); // Yield

            // Re-aquired implicitely by unlock() passing ownership?
            // If unlock sets m_owner = next, we are good.
        }
    }

    void SoftwareMutex::unlock()
    {
        if (!Scheduler::is_initialized()) {
            m_owner.clear();
            return;
        }

        VERIFY(is_executing_in_thread_mode());
        MaskedInterruptGuard interrupt_guard;
        ASSERT(m_lock);

        m_lock->lock();

        // Verify we own it?
        // ASSERT(m_owner == Scheduler::the().get_active_thread());

        if (m_waiting_threads.size() == 0) {
            m_owner.clear();
        } else {
            auto next_thread = m_waiting_threads.dequeue();
            m_owner = next_thread;
            next_thread->wakeup();
        }

        m_lock->unlock();
    }

    bool SoftwareMutex::is_locked()
    {
        if (!Scheduler::is_initialized())
            return false;

        MaskedInterruptGuard interrupt_guard;
        ASSERT(m_lock);

        // We can peek without lock if we just want a hint, but for correctness:
        m_lock->lock();
        bool locked = !m_owner.is_null();
        m_lock->unlock();

        return locked;
    }
}
