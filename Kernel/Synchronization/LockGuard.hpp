#pragma once

#include <Kernel/Synchronization/AbstractLock.hpp>

namespace Kernel
{
    class LockGuard
    {
    private:
        AbstractLock& m_lock;

    public:
        explicit LockGuard(AbstractLock& lock)
            : m_lock(lock)
        {
            m_lock.lock();
        }

        ~LockGuard()
        {
            m_lock.unlock();
        }

        LockGuard(const LockGuard&) = delete;
        LockGuard(LockGuard&&) = delete;

        LockGuard& operator=(const LockGuard&) = delete;
        LockGuard& operator=(LockGuard&&) = delete;
    };
}
