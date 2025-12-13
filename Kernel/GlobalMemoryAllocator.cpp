#include <Kernel/GlobalMemoryAllocator.hpp>
#include <Kernel/HandlerMode.hpp>
#include <Kernel/KernelMutex.hpp>
#include <Kernel/PageAllocator.hpp>
#include <Kernel/Synchronization/ScopedLock.hpp>

namespace Kernel {

GlobalMemoryAllocator::GlobalMemoryAllocator()
    : MemoryAllocator(allocate_heap()) {
  // Initialize Stack Smashing Protection
  init_stack_guard();
}

void GlobalMemoryAllocator::set_mutex_enabled(bool enabled) {
  malloc_mutex.set_enabled(enabled);
}

u8 *GlobalMemoryAllocator::allocate_eternal(usize size) {
  // Allocate without logging (debug_override=false)
  // Used for persistent structures that don't need tracking or can't fail
  // visibly.
  return allocate(size, false, __builtin_return_address(0));
}

Bytes GlobalMemoryAllocator::allocate_heap() {
  // Initial Kernel Heap (16KB).
  // The heap will grow dynamically as needed via allocate().
  m_heap = PageAllocator::the().allocate(power_of_two(16 * KiB)).must();
  dbgln("GlobalMemoryAllocator: Initialized with 16KB heap at {}",
        m_heap->bytes().data());
  return m_heap->bytes();
}

// Wrapper to ensure Thread Safety check and Mutex Locking
u8 *GlobalMemoryAllocator::allocate(usize size, bool debug_override,
                                    void *address) {
  VERIFY(Kernel::is_executing_in_thread_mode());

  Kernel::ScopedMutex lock(malloc_mutex);

  u8 *ptr = MemoryAllocator::allocate(size, debug_override, address);
  if (ptr)
    return ptr;

  // Heap exhausted. Try to expand.
  // We need at least size + sizeof(Node) bytes.
  // We'll allocate in 16KB chunks to reduce fragmentation/overhead.
  usize grow_size = 16 * KiB;
  if (size + 32 > grow_size) {
    // If request is huge, allocate enough for it (aligned to power of two).
    // We calculate nearest power of two > size
    usize power = 0;
    while ((1u << power) < (size + 128))
      power++;
    grow_size = 1 << power;
  }

  // Calculate power for PageAllocator
  usize power = 0;
  while ((1u << power) < grow_size)
    power++;

  auto block_opt = PageAllocator::the().allocate(power);
  if (!block_opt.is_valid()) {
    dbgln("GlobalMemoryAllocator: OOM! Failed to expand heap by {} bytes",
          1u << power);
    return nullptr;
  }

  auto &block = block_opt.value();
  Bytes bytes = block.bytes();

  dbgln("GlobalMemoryAllocator: Growing heap by {} bytes at {}", bytes.size(),
        bytes.data());
  add_memory(bytes);

  // Retry allocation
  return MemoryAllocator::allocate(size, debug_override, address);
}

void GlobalMemoryAllocator::deallocate(u8 *pointer, bool debug_override,
                                       void *address) {
  VERIFY(Kernel::is_executing_in_thread_mode());

  Kernel::ScopedMutex lock(malloc_mutex);
  MemoryAllocator::deallocate(pointer, debug_override, address);
}

u8 *GlobalMemoryAllocator::reallocate(u8 *pointer, usize size,
                                      bool debug_override, void *address) {
  VERIFY(Kernel::is_executing_in_thread_mode());

  Kernel::ScopedMutex lock(malloc_mutex);
  return MemoryAllocator::reallocate(pointer, size, debug_override, address);
}

} // namespace Kernel

// --- Standard C Library Overrides ---

extern "C" void *malloc(usize size) {
  return Kernel::GlobalMemoryAllocator::the().allocate(
      size, true, __builtin_return_address(0));
}

extern "C" void *calloc(usize nmembers, usize size) {
  usize total_size = nmembers * size;
  u8 *pointer = Kernel::GlobalMemoryAllocator::the().allocate(
      total_size, true, __builtin_return_address(0));

  if (pointer)
    __builtin_memset(pointer, 0, total_size);

  return pointer;
}

extern "C" void free(void *pointer) {
  return Kernel::GlobalMemoryAllocator::the().deallocate(
      reinterpret_cast<u8 *>(pointer), true, __builtin_return_address(0));
}

extern "C" void *realloc(void *pointer, usize size) {
  return Kernel::GlobalMemoryAllocator::the().reallocate(
      reinterpret_cast<u8 *>(pointer), size, true, __builtin_return_address(0));
}

extern "C" void *reallocarray(void *pointer, usize nmembers, usize size) {
  usize total_size;
  if (__builtin_mul_overflow(nmembers, size, &total_size)) {
    return nullptr; // errno = ENOMEM
  }

  return Kernel::GlobalMemoryAllocator::the().reallocate(
      reinterpret_cast<u8 *>(pointer), total_size, true,
      __builtin_return_address(0));
}
