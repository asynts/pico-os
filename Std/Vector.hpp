#pragma once

#include <Std/Forward.hpp>

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
            printf("Erasing %zu items.\n", m_size);
            for (usize i=0; i<m_size; ++i)
                m_data[i].~T();

            delete m_data;
        }

        void append(const T &value)
        {
            ensure_capacity(m_size + 1);

            new (m_data + m_size) T { value };

            ++m_size;
        }

        void append(T &&value)
        {
            ensure_capacity(m_size + 1);

            new (m_data + m_size) T;
            m_data[m_size] = move(value);

            ++m_size;
        }

    private:
        void ensure_capacity(usize capacity)
        {
            if (capacity <= m_capacity)
                return;

            usize new_capacity = round_to_power_of_two(capacity);
            assert(new_capacity > capacity && new_capacity > m_capacity);

            printf("Allocating space for %zu items.\n", new_capacity);
            T *new_data = reinterpret_cast<T*>(new u8[new_capacity * sizeof(T)]);

            printf("Moving %zu items over.\n", m_size);
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
