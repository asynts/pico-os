#pragma once

#include <Kernel/Synchronization/AbstractLock.hpp>

namespace Kernel {
template <typename LockType> class ScopedLock {
public:
  explicit ScopedLock(LockType &lock) : m_lock(lock) { m_lock.lock(); }

  ~ScopedLock() { m_lock.unlock(); }

  // Disable copy/move
  ScopedLock(const ScopedLock &) = delete;
  ScopedLock &operator=(const ScopedLock &) = delete;
  ScopedLock(ScopedLock &&) = delete;
  ScopedLock &operator=(ScopedLock &&) = delete;

private:
  LockType &m_lock;
};

using ScopedMutex = ScopedLock<AbstractLock>;
} // namespace Kernel
