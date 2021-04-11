#pragma once

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
        ~ArmedScopeGuard()
        {
            if (m_armed)
                m_callback();
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
