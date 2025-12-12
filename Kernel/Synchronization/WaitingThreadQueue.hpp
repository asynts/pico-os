#pragma once

#include <Kernel/Synchronization/HardwareSpinLock.hpp>
#include <Kernel/Threads/Thread.hpp>
#include <Std/CircularQueue.hpp>
#include <Std/RefPtr.hpp>

namespace Kernel
{
    class WaitingThreadQueue
    {
    public:
        WaitingThreadQueue() = default;

        // Must be called to assign the lock that protects this queue
        void initialize(HardwareSpinLock& lock);

        void enqueue(Thread& thread);
        RefPtr<Thread> dequeue();
        bool is_empty() const;
        usize size() const;

    private:
        HardwareSpinLock* m_lock = nullptr;
        CircularQueue<RefPtr<Thread>, 32> m_threads;
    };
}
