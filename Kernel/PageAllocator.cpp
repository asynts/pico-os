#include <Kernel/PageAllocator.hpp>
#include <Kernel/KernelMutex.hpp>
#include <Kernel/HandlerMode.hpp>


extern "C" u8 __pico_ram_start[];
extern "C" u8 __pico_ram_end[];
extern "C" u8 __pico_boot_ram_start[];
extern "C" u8 __pico_boot_ram_end[];

namespace Kernel
{
    OwnedPageRange::~OwnedPageRange()
    {
        if (m_range.is_valid())
            PageAllocator::the().deallocate(*this);
    }

    void PageAllocator::set_mutex_enabled(bool enabled)
    {
        page_allocator_mutex.set_enabled(enabled);
    }

    PageAllocator::PageAllocator()
    {
        for (auto& block : m_blocks.span().iter()) {
            block = nullptr;
        }

        // My custom linker script will allocate the first 8 KiB of RAM for statup.
        // The rest can be managed by this page allocator.

        m_blocks[power_of_two(128 * KiB)] = reinterpret_cast<Block*>(__pico_ram_start + 128 * KiB);
        m_blocks[power_of_two(64 * KiB)] = reinterpret_cast<Block*>(__pico_ram_start + 64 * KiB);
        m_blocks[power_of_two(32 * KiB)] = reinterpret_cast<Block*>(__pico_ram_start + 32 * KiB);
        m_blocks[power_of_two(16 * KiB)] = reinterpret_cast<Block*>(__pico_ram_start + 16 * KiB);
        m_blocks[power_of_two(8 * KiB)] = reinterpret_cast<Block*>(__pico_ram_start + 8 * KiB);

        for (auto& block : m_blocks.span().iter()) {
            VERIFY(bit_cast<uptr>(block) >= bit_cast<uptr>(__pico_boot_ram_end));
            VERIFY(bit_cast<uptr>(block) < bit_cast<uptr>(__pico_ram_end));

            // FIXME: We appear to assert when we dereference this pointer.
            block->m_next = nullptr;
        }
    }

    Optional<OwnedPageRange> PageAllocator::allocate(usize power)
    {
        VERIFY(is_executing_in_thread_mode());

        page_allocator_mutex.lock();
        Optional<PageRange> range_opt = allocate_locked(power);
        page_allocator_mutex.unlock();

        if (range_opt.is_valid()) {
            return OwnedPageRange { range_opt.value() };
        } else {
            return {};
        }
    }

    Optional<PageRange> PageAllocator::allocate_locked(usize power)
    {
        usize size = 1 << power;

        if (debug_page_allocator)
            dbgln("[PageAllocator::allocate] power={}", power);

        ASSERT(power <= max_power);

        if (m_blocks[power] != nullptr) {
            uptr base = reinterpret_cast<uptr>(m_blocks[power]);

            if (debug_page_allocator)
                dbgln("[PageAllocator::allocate] Found suitable block {}", base);

            m_blocks[power] = m_blocks[power]->m_next;
            return PageRange { power, base };
        }

        ASSERT(power < max_power);

        auto block_opt = allocate_locked(power + 1);
        if (!block_opt.is_valid()) {
            return {};
        }
        auto block = block_opt.value();

        deallocate_locked(PageRange{ power, block.m_base + size });

        return PageRange { power, block.m_base };
    }

    void PageAllocator::deallocate(OwnedPageRange& owned_range)
    {
        VERIFY(is_executing_in_thread_mode());

        PageRange range = owned_range.m_range.must();
        owned_range.m_range.clear();

        page_allocator_mutex.lock();
        deallocate_locked(range);
        page_allocator_mutex.unlock();
    }

    void PageAllocator::deallocate_locked(PageRange range)
    {
        if (debug_page_allocator)
            dbgln("[PageAllocator::deallocate] power={} base={}", range.m_power, range.m_base);

        ASSERT(range.m_power <= max_power);

        auto *block_ptr = reinterpret_cast<Block*>(range.m_base);
        block_ptr->m_next = m_blocks[range.m_power];
        m_blocks[range.m_power] = block_ptr;
    }
}
