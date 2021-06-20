#pragma once

#include <Std/Forward.hpp>
#include <Std/Format.hpp>

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

            if (m_pointer)
                m_pointer->ref();
        }
        RefPtr(RefPtr&& other)
        {
            m_pointer = exchange(other.m_pointer, nullptr);
        }
        ~RefPtr()
        {
            clear();
        }

        void clear()
        {
            if (m_pointer)
                m_pointer->unref();

            m_pointer = nullptr;
        }

        RefPtr& operator=(const RefPtr& other)
        {
            clear();

            m_pointer = other.m_pointer;

            if (m_pointer)
                m_pointer->ref();
            return *this;
        }
        RefPtr& operator=(RefPtr&& other)
        {
            clear();

            m_pointer = exchange(other.m_pointer, nullptr);
            return *this;
        }

        const T& must() const
        {
            VERIFY(m_pointer != nullptr);
            return *m_pointer;
        }
        T& must()
        {
            VERIFY(m_pointer != nullptr);
            return *m_pointer;
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
        RefCounted() = default;
        RefCounted(const RefCounted&) = delete;
        RefCounted(RefCounted&&) = delete;

        template<typename... Parameters>
        static RefPtr<T> construct(Parameters&&... parameters)
        {
            return RefPtr<T> { new T { forward<Parameters>(parameters)... } };
        }

        usize refcount() const
        {
            return m_refcount;
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

    template<typename T>
    struct Formatter<RefPtr<T>> : Formatter<T*> {
    };
}
