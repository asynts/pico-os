#pragma once

#include <Std/Forward.hpp>
#include <Std/Span.hpp>

namespace Std {
template <typename T, usize Size> class Array {
public:
  T __array[Size];

  [[nodiscard]] constexpr usize size() const { return Size; }
  [[nodiscard]] constexpr T *data() { return __array; }
  [[nodiscard]] constexpr const T *data() const { return __array; }

  [[nodiscard]] constexpr T &operator[](usize index) {
    // Note: Use at() for bounds checking
    return __array[index];
  }

  [[nodiscard]] constexpr const T &operator[](usize index) const {
    return __array[index];
  }

  [[nodiscard]] constexpr T &at(usize index) {
    ASSERT(index < Size);
    return __array[index];
  }
  [[nodiscard]] constexpr const T &at(usize index) const {
    ASSERT(index < Size);
    return __array[index];
  }

  T *begin() { return __array; }
  T *end() { return __array + Size; }
  const T *begin() const { return __array; }
  const T *end() const { return __array + Size; }

  Span<const T> span() const { return {__array, Size}; }
  Span<T> span() { return {__array, Size}; }
};
} // namespace Std
