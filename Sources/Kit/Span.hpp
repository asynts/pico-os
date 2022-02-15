#pragma once

#include <Kit/Forward.hpp>
#include <Kit/Assertions.hpp>
#include <Kit/Traits.hpp>

namespace Kit
{
    // FIXME: Move this into another header.
    template<typename T>
    T min(T lhs, T rhs)
    {
        if (lhs <= rhs)
            return lhs;
        return rhs;
    }

    template<typename T>
    struct Span {
        T *m_data;
        usize m_size;

        Span(T *data, usize size)
            : m_data(data)
            , m_size(size)
        {

        }

        usize copy_to(Span<Traits::RemoveConst<T>> span)
        {
            ASSERT(span.m_size >= m_size);
            memcpy(span.m_data, m_data, m_size);
            return m_size;
        }

        usize copy_trimmed_to(Span<Traits::RemoveConst<T>> span)
        {
            usize ncopied = min(span.m_size, m_size);
            memcpy(span.m_data, m_data, ncopied);
            return ncopied;
        }
    };

    using Bytes = Span<u8>;
    using ReadonlyBytes = Span<const u8>;

    inline ReadonlyBytes operator ""_b(const char *data, usize size)
    {
        return ReadonlyBytes { reinterpret_cast<const u8*>(data), size };
    }
}
