#pragma once

#include <Kernel/Synchronization/AbstractLock.hpp>
#include <Kernel/Synchronization/HardwareSpinLock.hpp>
#include <Std/CircularQueue.hpp>
#include <Std/RefPtr.hpp>
#include <Kernel/Threads/Thread.hpp>

namespace Kernel
{
    class SoftwareMutex final : public AbstractLock
    {
    public:
        SoftwareMutex() = default;

        void initialize(HardwareSpinLock& lock);

        virtual void lock() override;
        virtual void unlock() override;

        // API Compatibility for KernelMutex
        void set_enabled(bool enabled) { (void)enabled; }
        bool is_locked();

    private:
        HardwareSpinLock* m_lock = nullptr;
        CircularQueue<RefPtr<Thread>, 16> m_waiting_threads;
        RefPtr<Thread> m_owner;
    };
}
