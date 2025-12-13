#pragma once

#include <Kernel/Forward.hpp>
#include <Std/Span.hpp>

namespace Kernel {
// A helper class to manage pushing data onto a downward-growing stack.
// Useful for setting up thread stacks (arguments, environment, initial
// context).
struct StackWrapper {
  explicit StackWrapper(Bytes bytes)
      : m_bytes(bytes),
        m_top(bytes.data() + bytes.size()) // Start at the end (high address)
  {}

  // Reserve space on the stack by moving the top pointer down.
  // Returns the new pointer to the reserved space.
  u8 *reserve(usize count) {
    ASSERT(m_bytes.data() + count <= m_top);
    return m_top -= count;
  }

  // Push a raw byte buffer onto the stack.
  u8 *push(ReadonlyBytes bytes) {
    u8 *data = reserve(bytes.size());
    bytes.copy_to({data, bytes.size()});
    return data;
  }

  // Push a POD value onto the stack.
  template <typename T> T *push_value(const T &value) {
    u8 *data = reserve(sizeof(value));
    return new (data) T{value};
  }

  // Push a null-terminated string onto the stack.
  // Ensures the string pointer is aligned to 4 bytes for AAPCS compliance /
  // efficiency.
  char *push_cstring(const char *cstring) {
    usize length = __builtin_strlen(cstring) + 1; // +1 for null terminator
    reserve(length);

    // Align the pointer *downwards* to specific boundary (usually 4 for ARM
    // strings) This introduces padding *above* the string if necessary.
    char *cstring_on_stack = (char *)align(4);

    __builtin_strcpy(cstring_on_stack, cstring);
    return cstring_on_stack;
  }

  // Align the stack pointer downwards to the given boundary.
  // Boundary must be a power of two.
  u8 *align(u32 boundary) {
    static_assert(sizeof(u8 *) == sizeof(u32));
    uptr address = (uptr)m_top;
    if (address % boundary != 0) {
      reserve(address % boundary);
    }
    return m_top;
  }

  u8 *top() const { return m_top; }

private:
  Bytes m_bytes;
  u8 *m_top;
};
} // namespace Kernel
