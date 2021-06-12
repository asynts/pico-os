#include <Kernel/PageAllocator.hpp>

extern "C" u8 __end__[];
extern "C" u8 __HeapLimit[];

namespace Kernel
{
    OwnedPageRange::~OwnedPageRange()
    {
        if (m_range.is_valid())
            PageAllocator::the().deallocate(m_range.value());
    }

    PageAllocator::PageAllocator()
    {
        for (auto& block : m_blocks.span().iter()) {
            block = nullptr;
        }

        // FIXME: We should be able to manage the entire 32 KiB RAM in the page allocator

        dbgln("[PageAllocator::PageAllocator] Trying to find suitable block in {}-{}", __end__, __HeapLimit);

        // Try to grab as much memory as possible
        for (usize power = max_power; power > 0; --power) {
            if (__HeapLimit - __end__ > (1 << power + 1)) {
                usize size = 1 << power;

                dbgln("[PageAllocator::PageAllocator] Found suitable block size 2^{} = {}", power, size);

                uptr end = reinterpret_cast<uptr>(__HeapLimit);
                end -= end % size;

                uptr start = end - size;

                VERIFY(start % size == 0);
                VERIFY(start >= reinterpret_cast<uptr>(__end__));
                VERIFY(end <= reinterpret_cast<uptr>(__HeapLimit));

                dbgln("[PageAllocator::PageAllocator] Computed suitable block {}-{}", start, end);

                deallocate(PageRange{ power, start });

                return;
            }
        }

        VERIFY_NOT_REACHED();
    }
}
