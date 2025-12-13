#pragma once

#include <Std/CircularQueue.hpp>
#include <Std/Singleton.hpp>
#include <Std/Vector.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/HandlerMode.hpp>
#include <Kernel/PageAllocator.hpp>
#include <Kernel/Synchronization/HardwareSpinLock.hpp>

#include <Kernel/Threads/Thread.hpp>
#include <hardware/structs/sio.h>

namespace Kernel {
constexpr bool debug_scheduler = false;
constexpr bool scheduler_slow = false;

class Scheduler : public Singleton<Scheduler> {
public:
  Thread *get_active_thread_if_avaliable() {
    VERIFY(is_executing_in_handler_mode() || !are_interrupts_enabled());
    return m_active_threads[sio_hw->cpuid];
  }

  Thread &get_active_thread() {
    VERIFY(is_executing_in_handler_mode() || !are_interrupts_enabled());
    auto &thread = m_active_threads[sio_hw->cpuid];
    VERIFY(thread != nullptr);
    return *thread;
  }

  void clear_active_thread() {
    VERIFY(is_executing_in_handler_mode() || !are_interrupts_enabled());
    auto &thread = m_active_threads[sio_hw->cpuid];
    VERIFY(!thread.is_null());
    thread.clear();
  }

  Thread &schedule();

  void add_thread(RefPtr<Thread> thread) {
    VERIFY(is_executing_in_handler_mode() || !are_interrupts_enabled());
    m_queued_threads.enqueue(thread);
  }

  void dump();

  void loop();
  void trigger();

  bool m_enabled = false;

  // In thread mode, we must disable interrupts to interact with these.
  // For multi-thread support, we should add a mutex here.
  CircularQueue<RefPtr<Thread>, 16> m_queued_threads;
  CircularQueue<RefPtr<Thread>, 16> m_dangling_threads;
  Array<RefPtr<Thread>, 2> m_active_threads;

  HardwareSpinLock *m_lock = nullptr;

private:
  Array<RefPtr<Thread>, 2> m_default_threads;
  Array<RefPtr<Thread>, 2> m_fallback_threads;

  friend Singleton<Scheduler>;
  Scheduler(RefPtr<Thread> startup_thread, HardwareSpinLock &lock);

  RefPtr<Thread> choose_default_thread();
};
} // namespace Kernel
