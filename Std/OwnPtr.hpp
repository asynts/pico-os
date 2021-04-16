#pragma once

#include <Std/Forward.hpp>

namespace Std
{
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
        ~OwnPtr()
        {
            clear();
        }

        void clear()
        {
            delete m_pointer;
            m_pointer = nullptr;
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

    private:
        T *m_pointer;
    };

    template<typename T, typename... Parameters>
    OwnPtr<T> make(Parameters&&... parameters)
    {
        return new T { forward<Parameters>(parameters)... };
    }
}
