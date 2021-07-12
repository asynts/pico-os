#include <Kernel/PageAllocator.hpp>
#include <Kernel/KernelMutex.hpp>

extern "C" u8 __end__[];
extern "C" u8 __HeapLimit[];

namespace Kernel
{
    // Hack:    Defined here to prevent circular depencency:
    //          `Thread` -> `PageAllocator` -> `KernelMutex` -> `Thread`
    static KernelMutex page_allocator_mutex;

    OwnedPageRange::~OwnedPageRange()
    {
        if (m_range.is_valid())
            PageAllocator::the().deallocate(*this);
    }

    PageAllocator::PageAllocator()
    {
        for (auto& block : m_blocks.span().iter()) {
            block = nullptr;
        }

        // FIXME: We should be able to manage the entire 32 KiB RAM in the page allocator
        // FIXME: Wait, I thought we had 256 KiB RAM?

        // Try to grab as much memory as possible
        for (usize power = max_power; power > 0; --power) {
            if (__HeapLimit - __end__ >= 1 << (power + 1)) {
                usize size = 1 << power;

                uptr end = reinterpret_cast<uptr>(__HeapLimit);
                end -= end % size;

                uptr start = end - size;

                VERIFY(start % size == 0);
                VERIFY(start >= reinterpret_cast<uptr>(__end__));
                VERIFY(end <= reinterpret_cast<uptr>(__HeapLimit));

                deallocate_locked(PageRange { power, start });

                return;
            }
        }

        VERIFY_NOT_REACHED();
    }

    Optional<OwnedPageRange> PageAllocator::allocate(usize power)
    {
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
