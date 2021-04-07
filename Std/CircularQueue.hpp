#pragma once

#include <Std/Vector.hpp>

namespace Std
{
    template<typename T, usize Size>
    class CircularQueue {
    public:
        CircularQueue()
        {
            m_offset = 0;
            m_size = 0;
        }
        ~CircularQueue()
        {
            usize count = m_size;
            for (usize i = 0; i < count; ++i) {
                front().~T();
                --m_size;
            }
        }

        // FIXME: We want to be able to copy and move these
        CircularQueue(const CircularQueue&) = delete;
        CircularQueue(CircularQueue&&) = delete;
        CircularQueue& operator=(const CircularQueue&) = delete;
        CircularQueue& operator=(CircularQueue&&) = delete;

        void enqueue(const T& value)
        {
            return enqueue_impl(value);
        }
        void enqueue(T&& value)
        {
            return enqueue_impl(move(value));
        }

        T& front()
        {
            ASSERT(m_size > 0);

            isize offset = (m_offset - m_size + Size) % Size;
            return *reinterpret_cast<T*>(m_data + offset * sizeof(T));
        }

        T dequeue()
        {
            auto& target = front();

            T value = move(target);
            target.~T();

            --m_size;

            return value;
        }

        usize size() const { return m_size; }

    private:
        template<typename T_>
        void enqueue_impl(T_&& value)
        {
            ASSERT(m_size < Size);
            ASSERT(m_offset < Size);

            new (m_data + m_offset * sizeof(T)) T { forward<T_>(value) };
            m_offset = (m_offset + 1) % Size;

            ++m_size;
        }

        isize m_offset;
        isize m_size;
        u8 m_data[sizeof(T) * Size];
    };
}
