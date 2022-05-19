#pragma once

#include <Std/Forward.hpp>

#include <Kernel/Synchronization/AbstractLock.hpp>
#include <Kernel/Synchronization/MaskedInterruptGuard.hpp>

namespace Kernel
{
    class HardwareSpinLock final
        : public AbstractLock
    {
    private:
        volatile u32 *m_spin_lock_pointer;

    public:
        explicit HardwareSpinLock(volatile u32 *spin_lock_pointer)
            : m_spin_lock_pointer(spin_lock_pointer)
        {

        }

        virtual void lock() override
        {
            // We only want to synchronize with the other core, not with other threads.
            MaskedInterruptGuard interrupt_guard;

            while (true) {
                u32 value;
                __atomic_load(m_spin_lock_pointer, &value, __ATOMIC_SEQ_CST);

                // We read non-zero, if the lock was aquired sucessfully.
                if (value != 0)
                    continue;
            }
        }

        virtual void unlock() override
        {
            // We only want to synchronize with the other core, not with other threads.
            MaskedInterruptGuard interrupt_guard;

            // Writing anything will release the lock.
            u32 value = 1;
            __atomic_store(m_spin_lock_pointer, &value, __ATOMIC_SEQ_CST);
        }
    };
}
