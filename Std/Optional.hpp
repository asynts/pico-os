#pragma once

#include <Std/Forward.hpp>

namespace Std {
    template<typename T>
    class Optional {
    public:
        Optional()
        {
            m_is_valid = false;
        }
        Optional(T value)
        {
            new (m_value) T { value };
            m_is_valid = true;
        }

        bool is_valid() const { return m_is_valid; }

        T& value() { return *reinterpret_cast<T*>(m_value); }
        T value_or(T default_)
        {
            if (is_valid())
                return value();
            else
                return default_;
        }
        T& must()
        {
            VERIFY(is_valid());
            return value();
        }

    private:
        u8 m_value[sizeof(T)];
        bool m_is_valid;
    };
}
