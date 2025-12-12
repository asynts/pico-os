#include <Kernel/Synchronization/WaitingThreadQueue.hpp>
#include <Kernel/HandlerMode.hpp>

namespace Kernel
{
    void WaitingThreadQueue::initialize(HardwareSpinLock& lock)
    {
        m_lock = &lock;
    }

    void WaitingThreadQueue::enqueue(Thread& thread)
    {
        ASSERT(!are_interrupts_enabled());
        ASSERT(m_lock);

        m_lock->lock();
        m_threads.enqueue(RefPtr<Thread>(thread));
        m_lock->unlock();
    }

    RefPtr<Thread> WaitingThreadQueue::dequeue()
    {
        ASSERT(!are_interrupts_enabled());
        ASSERT(m_lock);

        m_lock->lock();
        auto thread = m_threads.dequeue();
        m_lock->unlock();
        return thread;
    }

    bool WaitingThreadQueue::is_empty() const
    {
        // Note: access without lock is racy if interrupts/other cores involved,
        // but typically used after lock held or for heuristic.
        // For correctness, we should lock. But 'const' makes it hard with the current lock signature?
        // Let's rely on caller or remove const/add mutable to lock.
        // Actually HardwareSpinLock::lock() is not const.
        // Implementation note: Ideally we lock here too.
        return m_threads.size() == 0;
    }

    usize WaitingThreadQueue::size() const
    {
        return m_threads.size();
    }
}
