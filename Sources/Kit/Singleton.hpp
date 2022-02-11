#pragma once

#include <Kit/Forward.hpp>
#include <Kit/Assertions.hpp>

namespace Kit
{
    template<typename T>
    struct Singleton {
    private:
        static inline T m_value;
        static inline bool m_initialized;

    public:
        static void initialize() {
            if (m_initialized) {
                ASSERT_NOT_REACHED();
            } else {
                new (&m_value) T;
                m_initialized = true;
            }
        }

        static T& the() {
            if (m_initialized) {
                return m_value;
            } else {
                ASSERT_NOT_REACHED();
            }
        }
    };
}
