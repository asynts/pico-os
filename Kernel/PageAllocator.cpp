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

                deallocate(PageRange{ power, start });

                return;
            }
        }

        VERIFY_NOT_REACHED();
    }
}
