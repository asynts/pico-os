#pragma once

#include <Std/Forward.hpp>

namespace Std
{
    template<typename T>
    class OwnPtr;

    template<typename T>
    class NonnullOwnPtr {
    public:
        friend OwnPtr<T>;

        NonnullOwnPtr(T *pointer)
            : m_pointer(pointer)
        {
        }
        NonnullOwnPtr(NonnullOwnPtr&& other)
        {
            m_pointer = nullptr;
            *this = move(other);
        }
        ~NonnullOwnPtr()
        {
            delete m_pointer;
        }

        const T* ptr() const { return m_pointer; }
        T* ptr() { return m_pointer; }

        const T& operator*() const { return *m_pointer; }
        T& operator*() { return *m_pointer; }

        const T* operator->() const { return m_pointer; }
        T* operator->() { return m_pointer; }

        NonnullOwnPtr& operator=(NonnullOwnPtr&& other)
        {
            delete m_pointer;

            m_pointer = exchange(other.m_pointer, nullptr);

            return *this;
        }

    private:
        T *m_pointer;
    };

    template<typename T>
    class OwnPtr {
    public:
        OwnPtr()
            : m_pointer(nullptr)
        {
        }
        OwnPtr(T *pointer)
            : m_pointer(pointer)
        {
        }
        OwnPtr(OwnPtr&& other)
        {
            m_pointer = nullptr;

            *this = move(other);
        }
        OwnPtr(NonnullOwnPtr<T>&& other)
        {
            m_pointer = nullptr;

            *this = move(other);
        }
        ~OwnPtr()
        {
            clear();
        }

        void clear()
        {
            delete m_pointer;
            m_pointer = nullptr;
        }

        T* leak()
        {
            return exchange(m_pointer, nullptr);
        }

        const T* ptr() const { return m_pointer; }
        T* ptr() { return m_pointer; }

        const T& operator*() const { return *m_pointer; }
        T& operator*() { return *m_pointer; }

        const T* operator->() const { return m_pointer; }
        T* operator->() { return m_pointer; }

        OwnPtr& operator=(OwnPtr&& other)
        {
            clear();

            m_pointer = exchange(other.m_pointer, nullptr);

            return *this;
        }
        OwnPtr& operator=(NonnullOwnPtr<T>&& other)
        {
            clear();

            m_pointer = exchange(other.m_pointer, nullptr);

            return *this;
        }
        OwnPtr& operator=(T *other)
        {
            clear();

            m_pointer = other;

            return *this;
        }

    private:
        T *m_pointer;
    };

    template<typename T, typename... Parameters>
    NonnullOwnPtr<T> make(Parameters&&... parameters)
    {
        return new T { forward<Parameters>(parameters)... };
    }
}
