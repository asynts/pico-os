#pragma once

#include <Std/Forward.hpp>
#include <Std/Optional.hpp>

#include <Kernel/Forward.hpp>

namespace Kernel::Interrupt
{
    bool disable_interrupts();
    void enable_interrupts();
    void restore_interrupts(bool);

    class InterruptGuard {
    public:
        InterruptGuard()
        {
            m_were_enabled = disable_interrupts();
        }
        ~InterruptGuard()
        {
            if (m_were_enabled.is_valid())
                restore_interrupts(m_were_enabled.value());
        }

        InterruptGuard(const InterruptGuard&) = delete;
        InterruptGuard(InterruptGuard&& other)
        {
            m_were_enabled = exchange(other.m_were_enabled, {});
        }

        InterruptGuard& operator=(const InterruptGuard&) = delete;
        InterruptGuard& operator=(InterruptGuard&& other)
        {
            VERIFY(other.m_were_enabled.is_valid());

            m_were_enabled = other.m_were_enabled;
            other.m_were_enabled.clear();

            return *this;
        }

    private:
        Optional<bool> m_were_enabled;
    };
}
