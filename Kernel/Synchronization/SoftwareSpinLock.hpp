#pragma once

#include <Kernel/Synchronization/AbstractLock.hpp>
#include <Kernel/Synchronization/HardwareSpinLock.hpp>
#include <Std/Forward.hpp>

namespace Kernel
{
    class SoftwareSpinLock final : public AbstractLock
    {
    public:
        SoftwareSpinLock() = default;

        virtual void lock() override;
        virtual void unlock() override;

        static void initialize(HardwareSpinLock& hardware_lock);

    private:
        volatile bool m_locked = false;
        volatile i32 m_owning_core = -1;
        static HardwareSpinLock* s_hardware_lock;
    };
}
