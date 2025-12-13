#pragma once

#include <Std/Concepts.hpp>
#include <Std/Forward.hpp>
#include <Std/Span.hpp>

namespace Std {
template <typename T, usize InlineSize = 0> class Vector {
public:
  // -- Constructors & Destructor --

  constexpr Vector()
      : m_size(0), m_capacity(InlineSize), m_use_inline_data(true),
        m_data(nullptr) {}

  ~Vector() {
    clear();
    if (!m_use_inline_data && m_data) {
      operator delete[](m_data);
    }
  }

  Vector(const Vector &other) : Vector() { *this = other; }

  Vector(Vector &&other) noexcept : Vector() { *this = move(other); }

  // -- Assignments --

  Vector &operator=(const Vector &other) {
    if (this == &other)
      return *this;
    clear();
    ensure_capacity(other.size());
    for (usize i = 0; i < other.size(); ++i) {
      new (data() + i) T(other[i]);
    }
    m_size = other.size();
    return *this;
  }

  Vector &operator=(Vector &&other) noexcept {
    if (this == &other)
      return *this;
    clear();

    if (other.m_use_inline_data) {
      // If other is using inline data, we must copy it because we can't "steal"
      // the buffer embedded in the other object.
      ensure_capacity(other.size());
      for (usize i = 0; i < other.size(); ++i) {
        new (data() + i) T(move(other[i]));
      }
      m_size = other.size();
    } else {
      // Steal the heap pointer
      if (!m_use_inline_data)
        operator delete[](m_data);

      m_data = other.m_data;
      m_size = other.m_size;
      m_capacity = other.m_capacity;
      m_use_inline_data = false;

      // Reset other
      other.m_data = nullptr;
      other.m_size = 0;
      other.m_capacity = InlineSize;
      other.m_use_inline_data = true;
    }
    other.clear();
    return *this;
  }

  // -- Element Access --

  T &append(const T &value) {
    ensure_capacity(m_size + 1);
    new (data() + m_size) T(value);
    return data()[m_size++];
  }

  T &append(T &&value) {
    ensure_capacity(m_size + 1);
    new (data() + m_size) T(move(value));
    return data()[m_size++];
  }

  void extend(Span<const T> values) {
    ensure_capacity(m_size + values.size());
    for (auto &value : values.iter()) {
      append(value);
    }
  }

  [[nodiscard]] const T &operator[](usize index) const {
    ASSERT(index < m_size);
    return data()[index];
  }

  [[nodiscard]] T &operator[](usize index) {
    ASSERT(index < m_size);
    return data()[index];
  }

  [[nodiscard]] T &at(usize index) {
    // TODO: Throw exception or panic if out of bounds (ASSERT for now)
    ASSERT(index < m_size);
    return data()[index];
  }

  // -- Capacity & Size --

  [[nodiscard]] usize size() const { return m_size; }
  [[nodiscard]] usize capacity() const { return m_capacity; }
  [[nodiscard]] bool is_empty() const { return m_size == 0; }

  void ensure_capacity(usize needed_capacity) {
    if (m_capacity >= needed_capacity)
      return;

    usize new_capacity = m_capacity == 0 ? 4 : m_capacity;
    while (new_capacity < needed_capacity) {
      new_capacity *= 2;
    }

    T *new_buffer = static_cast<T *>(operator new[](new_capacity * sizeof(T)));

    // Move existing elements
    for (usize i = 0; i < m_size; ++i) {
      new (new_buffer + i) T(move(data()[i]));
      data()[i].~T();
    }

    if (!m_use_inline_data && m_data) {
      operator delete[](m_data);
    }

    m_data = new_buffer;
    m_capacity = new_capacity;
    m_use_inline_data = false;
  }

  void clear() {
    for (usize i = 0; i < m_size; ++i) {
      data()[i].~T();
    }
    m_size = 0;
  }

  // -- Iterators --

  T *begin() { return data(); }
  T *end() { return data() + m_size; }
  const T *begin() const { return data(); }
  const T *end() const { return data() + m_size; }

  Span<T> span() { return {data(), size()}; }
  Span<const T> span() const { return {data(), size()}; }
  SpanIterator<T> iter() { return span().iter(); }
  SpanIterator<const T> iter() const { return span().iter(); }

  T *data() {
    return m_use_inline_data ? reinterpret_cast<T *>(m_inline_data) : m_data;
  }
  const T *data() const {
    return m_use_inline_data ? reinterpret_cast<const T *>(m_inline_data)
                             : m_data;
  }

private:
  usize m_size;
  usize m_capacity;
  bool m_use_inline_data;

  T *m_data;
  alignas(T) u8 m_inline_data[sizeof(T) * (InlineSize > 0 ? InlineSize : 1)];
};
} // namespace Std
