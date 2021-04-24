#pragma once

#include <Std/Forward.hpp>

namespace Std
{
    template<typename Callback>
    class ArmedScopeGuard
    {
    public:
        ArmedScopeGuard(Callback&& callback)
            : m_callback(move(callback))
        {
        }
        ArmedScopeGuard(ArmedScopeGuard&& other)
            : m_armed(exchange(other.m_armed, false))
            , m_callback(move(other.m_callback))
        {
        }

        ~ArmedScopeGuard()
        {
            if (m_armed)
                m_callback();
        }

        ArmedScopeGuard(const ArmedScopeGuard&) = delete;
        ArmedScopeGuard& operator=(const ArmedScopeGuard&) = delete;

        ArmedScopeGuard& operator=(ArmedScopeGuard&& other)
        {
            VERIFY(!m_armed);

            m_armed = exchange(other.m_armed, false);
            m_callback = move(other.m_callback);

            return *this;
        }

        void disarm()
        {
            m_armed = false;
        }

    private:
        Callback m_callback;
        bool m_armed = true;
    };
}
