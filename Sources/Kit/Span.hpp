#pragma once

#include <Kit/Forward.hpp>

namespace Kit
{
    template<typename T>
    struct Span {
        T *m_data;
        usize m_size;

        Span(T *data, usize size)
            : m_data(data)
            , m_size(size)
        {

        }
    };

    using Bytes = Span<u8>;
    using ReadonlyBytes = Span<const u8>;

    inline ReadonlyBytes operator ""_b(const char *data, usize size)
    {
        return ReadonlyBytes { reinterpret_cast<const u8*>(data), size };
    }
}
