#pragma once

#include <Std/Forward.hpp>

// FIXME: Move constructors

// FIXME: Assignment operators

namespace Std
{
    template<typename T>
    class RefCounted;

    template<typename T>
    class RefPtr {
    public:
        RefPtr()
            : m_pointer(nullptr)
        {
        }
        RefPtr(nullptr_t)
            : m_pointer(nullptr)
        {
        }
        RefPtr(const RefPtr& other)
        {
            m_pointer = other.m_pointer;

            VERIFY(m_pointer);
            m_pointer->ref();
        }
        ~RefPtr()
        {
            if (m_pointer)
                m_pointer->unref();
        }

        operator const T*() const { return m_pointer; }
        operator T*() { return m_pointer; }

        const T* operator->() const { return m_pointer; }
        T* operator->() { return m_pointer; }

    private:
        T *m_pointer;

        friend RefCounted<T>;
        explicit RefPtr(T *pointer)
            : m_pointer(pointer)
        {
            m_pointer->ref();
        }
    };

    template<typename T>
    class RefCounted {
    public:
        template<typename... Parameters>
        static RefPtr<T> construct(Parameters&&... parameters)
        {
            return RefPtr<T> { new T { forward<Parameters>(parameters)... } };
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
