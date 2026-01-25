#include <Kernel/PageAllocator.hpp>
#include <Kernel/KernelMutex.hpp>
#include <Kernel/HandlerMode.hpp>

extern "C" u8 __heap_start[];
extern "C" u8 __heap_end[];

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

    void PageAllocator::discover_blocks(uptr area_start, uptr area_end)
    {
        // We need to align the start address which ensures the alignment of all blocks
        // Because the block size is a multiple of the page size
        area_start = round_to_alignment(area_start, page_size);

        for (usize block_power = max_power; block_power >= page_power; --block_power)
        {
            usize block_size = 1 << block_power;
            uptr block_start = area_start;
            uptr block_end = area_start + block_size;

            if (block_end <= area_end)
            {
                VERIFY(block_start % (1 << page_power) == 0);
                deallocate_locked(PageRange{ .m_power = block_power, .m_base = block_start });
                area_start = block_end;
            }
        }
    }

    void PageAllocator::dump()
    {
        for (usize block_power = 0; block_power < max_power; ++block_power)
        {
            Block *block = m_blocks[block_power];

            while (block != nullptr)
            {
                dbgln("[PageAllocator::print_blocks] block_power={}, block={}",
                    block_power, block);

                block = block->m_next;
            }
        }
    }

    PageAllocator::PageAllocator()
    {
        for (auto& block : m_blocks.span().iter()) {
            block = nullptr;
        }

        discover_blocks(
            reinterpret_cast<uptr>(__heap_start),
            reinterpret_cast<uptr>(__heap_end));
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

        // TODO Try to merge blocks back together
    }
}
