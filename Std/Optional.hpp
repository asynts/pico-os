#pragma once

#include <Std/Forward.hpp>

namespace Std {
    template<typename T>
    class Optional {
    public:
        Optional()
        {
            m_valid = false;
        }
        Optional(T value)
        {
            new (m_value) T { value };
            m_valid = true;
        }

        bool valid() const { return m_valid; }

        T value() { return *reinterpret_cast<T*>(m_value); }
        T value_or(T default_)
        {
            if (valid())
                return value();
            else
                return default_;
        }

    private:
        u8 m_value[sizeof(T)];
        bool m_valid;
    };
}
