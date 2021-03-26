#pragma once

#include <Std/Forward.hpp>
#include <Std/Span.hpp>

#include <pico/printf.h>

namespace Std {
    template<typename T>
    class Vector {
    public:
        Vector()
        {
            m_data = nullptr;
            m_capacity = 0;
            m_size = 0;
        }
        ~Vector()
        {
            for (usize i=0; i<m_size; ++i)
                m_data[i].~T();

            delete m_data;
        }

        template<typename T_>
        void append(T_&& value)
        {
            ensure_capacity(m_size + 1);

            new (m_data + m_size) T { forward<T_>(value) };

            ++m_size;
        }

        void extend(Span<const T> values)
        {
            ensure_capacity(m_size + values.size());

            for (auto& value : values.iter())
                append(value);
        }

        bool is_empty() const { return m_size > 0; }

        const T* data() const { return m_data; }
        T* data() { return m_data; }

        usize size() const { return m_size; }

        const T& operator[](usize index) const { return m_data[index]; }
        T& operator[](usize index) { return m_data[index]; }

        Span<const T> span() const { return { data(), size() }; }
        Span<T> span() { return { data(), size() }; }

        SpanIterator<const T> iter() const { return span(); }
        SpanIterator<T> iter() { return span(); }

    private:
        void ensure_capacity(usize capacity)
        {
            if (capacity <= m_capacity)
                return;

            printf("[ensure_capacity] capacity=%zu m_capacity=%zu m_size=%zu m_data=%p\n", capacity, m_capacity, m_size, (void*)m_data);

            usize new_capacity = round_to_power_of_two(capacity);
            VERIFY(new_capacity >= capacity && new_capacity > m_capacity);

            T *new_data = reinterpret_cast<T*>(new u8[new_capacity * sizeof(T)]);

            for (usize i=0; i<m_size; ++i)
                new (new_data + i) T { move(m_data[i]) };

            delete m_data;

            m_data = new_data;
            m_capacity = new_capacity;
        }

        T *m_data;
        usize m_capacity;
        usize m_size;
    };
}
