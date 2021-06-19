#pragma once

#include <Std/Forward.hpp>

// FIXME: Implement move, or rather, RefPtr<T>

namespace Std
{
    template<typename T>
    class RefCounted;

    template<typename T>
    class NonnullRefPtr {
    public:
        ~NonnullRefPtr()
        {
            m_pointer->unref();
        }
        NonnullRefPtr(const NonnullRefPtr& other)
        {
            m_pointer = other.m_pointer;

            VERIFY(m_pointer);
            m_pointer->ref();
        }

        operator const T*() const { return m_pointer; }
        operator T*() { return m_pointer; }

        const T* operator->() const { return m_pointer; }
        T* operator->() { return m_pointer; }

    private:
        T *m_pointer;

        friend RefCounted<T>;
        explicit NonnullRefPtr(T *pointer)
            : m_pointer(pointer)
        {
            m_pointer->ref();
        }
    };

    template<typename T>
    class RefCounted {
    public:
        template<typename... Parameters>
        static NonnullRefPtr<T> construct(Parameters&&... parameters)
        {
            return NonnullRefPtr<T> { new T { forward<Parameters>(parameters)... } };
        }

        void ref()
        {
            ++m_refcount;
        }

        void unref()
        {
            VERIFY(m_refcount > 0);
            --m_refcount;

            if (m_refcount == 0) {
                delete static_cast<T*>(this);
            }
        }

    private:
        usize m_refcount = 0;
    };
}
