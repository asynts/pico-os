#include <Kernel/Synchronization/SoftwareSpinLock.hpp>
#include <Kernel/HandlerMode.hpp>
#include <hardware/structs/sio.h>

namespace Kernel
{
    HardwareSpinLock* SoftwareSpinLock::s_hardware_lock = nullptr;

    void SoftwareSpinLock::initialize(HardwareSpinLock& hardware_lock)
    {
        s_hardware_lock = &hardware_lock;
    }

    static inline u32 get_core_num()
    {
        return sio_hw->cpuid;
    }

    void SoftwareSpinLock::lock()
    {
        // Must be used with interrupts disabled to prevent deadlocks with ISRs on the same core
        ASSERT(!are_interrupts_enabled());
        ASSERT(s_hardware_lock != nullptr);

        u32 current_core = get_core_num();

        // Deadlock detection: Check if we already own the lock
        if (m_owning_core == (i32)current_core) {
            // Panic or crash
            ASSERT(false && "Deadlock detected: SoftwareSpinLock already held by this core!");
        }

        while (true) {
            s_hardware_lock->lock();

            if (!m_locked) {
                m_locked = true;
                m_owning_core = (i32)current_core;
                s_hardware_lock->unlock();
                return;
            }

            s_hardware_lock->unlock();

            // Wait for Event (sleep until SEV from unlock)
            asm volatile("wfe");
        }
    }

    void SoftwareSpinLock::unlock()
    {
        ASSERT(!are_interrupts_enabled());
        ASSERT(s_hardware_lock != nullptr);

        s_hardware_lock->lock();

        ASSERT(m_locked);
        ASSERT(m_owning_core == (i32)get_core_num());

        m_locked = false;
        m_owning_core = -1;

        s_hardware_lock->unlock();

        // Signal other cores that the lock is free
        asm volatile("sev");
    }
}
