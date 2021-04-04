#pragma once

#ifdef TEST
# include <stdio.h>
#else
# include <pico/printf.h>
#endif

#include <Std/Forward.hpp>
#include <Std/Span.hpp>

namespace Std
{
    template<typename T, usize InlineSize = 0>
    class Vector {
    public:
        Vector()
        {
            m_use_inline_data = true;
            m_size = 0;
            m_capacity = InlineSize;
            m_data = nullptr;
        }
        ~Vector()
        {
            clear();

            operator delete[](m_data);
            m_data = nullptr;
        }
        Vector(const Vector& other)
            : Vector()
        {
            *this = other;
        }
        Vector(Vector&& other)
            : Vector()
        {
            *this = move(other);
        }

        template<typename T_ = T>
        void append(T_&& value)
        {
            ensure_capacity(m_size + 1);

            new (data() + m_size) T { forward<T_>(value) };
            ++m_size;
        }

        void extend(Span<const T> values)
        {
            ensure_capacity(m_size + values.size());

            for (auto& value : values.iter())
                append(value);
        }

        void ensure_capacity(usize new_capacity)
        {
            if (m_capacity >= new_capacity)
                return;

            new_capacity = round_to_power_of_two(new_capacity);

            T *new_data = reinterpret_cast<T*>(new u8[sizeof(T) * new_capacity]);

            for (usize i=0; i<m_size; ++i)
            {
                new (new_data + i) T { move(m_data[i]) };
                m_data[i].~T();
            }

            operator delete[](m_data);

            m_data = new_data;
            m_capacity = new_capacity;
            m_use_inline_data = false;
        }

        const T* data() const
        {
            if (m_use_inline_data)
                return reinterpret_cast<const T*>(m_inline_data);
            return m_data;
        }
        T* data()
        {
            if (m_use_inline_data)
                return reinterpret_cast<T*>(m_inline_data);
            return m_data;
        }

        usize size() const { return m_size; }
        usize capacity() const { return m_capacity; }

        Span<T> span() { return { data(), size() }; }
        Span<const T> span() const { return { data(), size() }; }

        SpanIterator<T> iter() { return span().iter(); }
        SpanIterator<const T> iter() const { return span().iter(); }

        const T& operator[](usize index) const { return data()[index]; }
        T& operator[](usize index) { return data()[index]; }

        void clear()
        {
            for (usize index = 0; index < m_size; ++index)
                data()[index].~T();

            m_size = 0;
        }

        Vector& operator=(const Vector& other)
        {
            clear();
            extend(other.span());
            return *this;
        }
        Vector& operator=(Vector&& other)
        {
            clear();

            if (other.m_use_inline_data) {
                extend(other.span());
                other.clear();
            } else {
                if (m_data)
                    operator delete[](m_data);

                m_use_inline_data = false;
                m_capacity = other.m_capacity;
                m_size = other.m_size;
                m_data = other.m_data;

                other.m_use_inline_data = true;
                other.m_capacity = InlineSize;
                other.m_size = 0;
                other.m_data = nullptr;
            }

            return *this;
        }

    private:
        bool m_use_inline_data;
        usize m_size;
        usize m_capacity;

        u8 m_inline_data[sizeof(T) * InlineSize];
        T *m_data;
    };
}
