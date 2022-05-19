#pragma once

namespace Kernel
{
    class MaskedInterruptGuard {
    public:
        MaskedInterruptGuard()
        {
            m_should_reenable_interrupts = disable_interrupts();
        }

        ~MaskedInterruptGuard()
        {
            if (m_should_reenable_interrupts) {
                VERIFY(!are_interrupts_enabled());
                enable_interrupts();
            }
        }

        MaskedInterruptGuard(const MaskedInterruptGuard&) = delete;
        MaskedInterruptGuard(MaskedInterruptGuard&&) = delete;

        void release_early()
        {
            if (m_should_reenable_interrupts) {
                m_should_reenable_interrupts = false;

                VERIFY(!are_interrupts_enabled());
                enable_interrupts();
            }
        }

    private:
        bool m_should_reenable_interrupts = false;
    };
}
