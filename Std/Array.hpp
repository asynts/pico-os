#pragma once

#include <Std/Forward.hpp>
#include <Std/Span.hpp>

namespace Std {
    template<typename T, usize Size>
    class Array {
    public:
        T __array[Size];

        Span<const T> span() const { return { __array, Size }; }
        Span<T> span() { return { __array, Size }; }
    };
}
