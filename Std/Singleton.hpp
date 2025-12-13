#pragma once

#include <Std/Forward.hpp>

namespace Std {
// Storage container to verify Type completeness on usage, not definition
template <typename T> struct SingletonContainer {
  static inline bool m_initialized = false;
  alignas(T) static inline u8 m_instance_storage[sizeof(T)];
};

// A singleton wrapper that manages the lifetime of a single instance.
//
// NOTE: This implementation is NOT thread-safe during initialization.
// It is intended to be initialized during the single-threaded boot phase
// of the kernel, before any secondary cores or interrupts are enabled.
// Access via `the()` is safe provided initialization is complete and
// the instance itself handles concurrency (or is immutable).
template <typename T> class Singleton {
public:
  Singleton(const Singleton &) = delete;
  Singleton(Singleton &&) = delete;
  Singleton &operator=(const Singleton &) = delete;
  Singleton &operator=(Singleton &&) = delete;

  Singleton() = default;

  template <typename... Parameters>
  static void initialize(Parameters &&...parameters) {
    VERIFY(!m_initialized);
    // Construct in-place
    new (SingletonContainer<T>::m_instance_storage)
        T{forward<Parameters>(parameters)...};
    m_initialized = true;
  }

  static bool is_initialized() { return m_initialized; }

  static T &the() {
    VERIFY(m_initialized);
    return *reinterpret_cast<T *>(SingletonContainer<T>::m_instance_storage);
  }

private:
  static inline bool &m_initialized = SingletonContainer<T>::m_initialized;
};
} // namespace Std
