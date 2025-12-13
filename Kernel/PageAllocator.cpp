#include <Kernel/HandlerMode.hpp>
#include <Kernel/KernelMutex.hpp>
#include <Kernel/PageAllocator.hpp>

#define __pico_ram_start ((u8 *)0x20000000)
#define __pico_ram_end ((u8 *)(0x20000000 + 264 * 1024))
#define __pico_boot_ram_start ((u8 *)0x20000000)
#define __pico_boot_ram_end ((u8 *)(0x20000000 + 8 * 1024))

namespace Kernel {
OwnedPageRange::~OwnedPageRange() {
  if (m_range.is_valid())
    PageAllocator::the().deallocate(*this);
}

void PageAllocator::set_mutex_enabled(bool enabled) {
  page_allocator_mutex.set_enabled(enabled);
}

PageAllocator::PageAllocator() {
  for (auto &block : m_blocks)
    block = nullptr;

  // The RP2040 has 264KB of RAM.
  // The first 256KB is contiguous in address space (0x20000000 - 0x20040000).
  // The last 8KB is striped/scratch banks (0x20040000 ...).
  //
  // Our linker script reserves the first 8 KB (0x20000000 - 0x20002000) for
  // the bootloader (stage2), vector table, and static kernel data (.data/.bss).
  //
  // We initialize the Buddy System with the remaining free memory.
  // We explicitly push free blocks of decreasing powers of two to cover the
  // range.
  //
  // Layout covered:
  //   [8KB, 16KB)   -> 8KB block
  //   [16KB, 32KB)  -> 16KB block
  //   [32KB, 64KB)  -> 32KB block
  //   [64KB, 128KB) -> 64KB block
  //   [128KB, 256KB)-> 128KB block
  //
  // Total managed: 256KB - 8KB = 248KB.
  //
  // Note: The scratch banks (top 8KB) are currently NOT managed by
  // PageAllocator. They are used for .stack sections in the linker script.

  // Helper helper to add a block
  auto add_initial_block = [&](usize offset, usize size) {
    m_blocks[power_of_two(size)] =
        reinterpret_cast<Block *>(__pico_ram_start + offset);
  };

  add_initial_block(128 * KiB, 128 * KiB);
  add_initial_block(64 * KiB, 64 * KiB);
  add_initial_block(32 * KiB, 32 * KiB);
  add_initial_block(16 * KiB, 16 * KiB);
  add_initial_block(8 * KiB, 8 * KiB);

  // Validate blocks
  for (auto *block : m_blocks) {
    if (!block)
      continue;

    // Ensure properly initialized
    block->m_next = nullptr;

    // Sanity check bounds
    uptr addr = bit_cast<uptr>(block);
    VERIFY(addr >= bit_cast<uptr>(__pico_boot_ram_end));
    VERIFY(addr < bit_cast<uptr>(__pico_ram_end));
  }
}

Optional<OwnedPageRange> PageAllocator::allocate(usize power) {
  VERIFY(is_executing_in_thread_mode());

  page_allocator_mutex.lock();
  Optional<PageRange> range_opt = allocate_locked(power);
  page_allocator_mutex.unlock();

  if (range_opt.is_valid()) {
    m_allocated_pages.insert(range_opt.value());
    return OwnedPageRange{range_opt.value()};
  } else {
    return {};
  }
}

Optional<PageRange> PageAllocator::allocate_locked(usize power) {
  usize size = 1 << power;

  if (debug_page_allocator)
    dbgln("[PageAllocator::allocate] power={}", power);

  ASSERT(power <= max_power);

  if (m_blocks[power] != nullptr) {
    uptr base = reinterpret_cast<uptr>(m_blocks[power]);

    if (debug_page_allocator)
      dbgln("[PageAllocator::allocate] Found suitable block {}", base);

    m_blocks[power] = m_blocks[power]->m_next;
    return PageRange{power, base};
  }

  ASSERT(power < max_power);

  auto block_opt = allocate_locked(power + 1);
  if (!block_opt.is_valid()) {
    return {};
  }
  auto block = block_opt.value();

  deallocate_locked(PageRange{power, block.m_base + size});

  return PageRange{power, block.m_base};
}

void PageAllocator::deallocate(OwnedPageRange &owned_range) {
  VERIFY(is_executing_in_thread_mode());

  PageRange range = owned_range.m_range.must();
  owned_range.m_range.clear();

  page_allocator_mutex.lock();
  m_allocated_pages.remove(range);
  deallocate_locked(range);
  page_allocator_mutex.unlock();
}

void PageAllocator::deallocate_locked(PageRange range) {
  if (debug_page_allocator)
    dbgln("[PageAllocator::deallocate] power={} base={}", range.m_power,
          range.m_base);

  ASSERT(range.m_power <= max_power);

  // Try to coalesce with buddy
  if (range.m_power < max_power) {
    usize size = 1 << range.m_power;
    uptr buddy_addr = range.m_base ^ size;

    // Check if buddy is in the free list for this power
    Block *prev = nullptr;
    Block *current = m_blocks[range.m_power];

    while (current) {
      if (reinterpret_cast<uptr>(current) == buddy_addr) {
        // Buddy is free! Coalesce.
        if (debug_page_allocator)
          dbgln("[PageAllocator::deallocate] Coalescing {} with buddy {}",
                range.m_base, buddy_addr);

        // Remove buddy from list
        if (prev) {
          prev->m_next = current->m_next;
        } else {
          m_blocks[range.m_power] = current->m_next;
        }

        // Recurse with combined block
        // The combined block starts at the lower of the two addresses
        uptr combined_base =
            (range.m_base < buddy_addr) ? range.m_base : buddy_addr;
        deallocate_locked(PageRange{range.m_power + 1, combined_base});
        return;
      }
      prev = current;
      current = current->m_next;
    }
  }

  auto *block_ptr = reinterpret_cast<Block *>(range.m_base);
  block_ptr->m_next = m_blocks[range.m_power];
  m_blocks[range.m_power] = block_ptr;
}
} // namespace Kernel
