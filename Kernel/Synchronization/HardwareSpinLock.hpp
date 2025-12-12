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
                // Reading from the spin lock register attempts to claim it.
                // A non-zero value means we successfully claimed the lock (value is the lock bitmap).
                // A zero value means the lock was already claimed by someone else.
                u32 value = *m_spin_lock_pointer;

                if (value != 0) {
                    // We got the lock!
                    // Full memory barrier to ensure subsequent operations verify against the lock acquisition.
                    __sync_synchronize();
                    return;
                }

                // We failed to get the lock.
                // Wait for an event (SEV) from the other core to save power before retrying.
                asm volatile("wfe");
            }
        }

        virtual void unlock() override
        {
            // We only want to synchronize with the other core, not with other threads.
            MaskedInterruptGuard interrupt_guard;

            // Full memory barrier to ensure all operations finishing before we release the lock.
            __sync_synchronize();

            // Writing anything will release the lock.
            *m_spin_lock_pointer = 1;

            // Signal the other core that the lock is free.
            asm volatile("sev");
        }
    };
}
