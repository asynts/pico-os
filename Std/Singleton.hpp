#pragma once

#include <Std/Forward.hpp>

// FIXME: This is not safe for multiple cores or even multiple threads

namespace Std
{
    template<typename T>
    struct SingletonContainer
    {
        static inline bool m_initialized = false;

        alignas(T)
        static inline u8 m_instance[sizeof(T)];
    };

    template<typename T>
    class Singleton {
    public:
        Singleton(const Singleton&) = delete;
        Singleton(Singleton&&) = delete;

        Singleton() = default;

        template<typename... Parameters>
        static void initialize(Parameters&&... parameters)
        {
            VERIFY(!m_initialized);

            m_initialized = true;
            new (m_instance) T { forward<Parameters>(parameters)... };
        }

        static bool is_initialized()
        {
            return m_initialized;
        }

        static T& the()
        {
            VERIFY(m_initialized);
            return *m_instance;
        }

    private:
        static inline bool& m_initialized = SingletonContainer<T>::m_initialized;

        // FIXME: This is the incorrect type
        static inline T (&m_instance)[] = reinterpret_cast<T(&)[]>(SingletonContainer<T>::m_instance);
    };
}
