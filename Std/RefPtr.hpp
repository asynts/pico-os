#pragma once

#include <Std/Forward.hpp>

namespace Std
{
    template<typename T>
    class RefCounted;

    struct DanglingObjectMarker {
    };

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
        RefPtr(T& other)
            : m_pointer(&other)
        {
            m_pointer->ref();
        }
        RefPtr(T& other, DanglingObjectMarker)
            : m_pointer(&other)
        {
            // We do not reference here since dangling objects have an initial reference count of one.
            VERIFY(m_pointer->refcount() == 1);
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

        bool is_null() const
        {
            return m_pointer == nullptr;
        }

        void clear()
        {
            if (m_pointer)
                m_pointer->unref();

            m_pointer = nullptr;
        }

        const T* ptr() const { return m_pointer; }
        T* ptr() { return m_pointer; }

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
            return RefPtr<T> { *new T { forward<Parameters>(parameters)... }, DanglingObjectMarker{} };
        }

        usize refcount() const
        {
            return m_refcount;
        }

        void ref()
        {
            VERIFY(m_refcount >= 1);
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
        usize m_refcount = 1;
    };
}
