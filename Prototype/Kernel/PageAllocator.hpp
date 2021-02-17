#pragma once

namespace Kernel {
    class PageRange {
    public:
        PageRange(flatptr_t base, usize order);

        usize order() const;
        usize size() const;
        usize pages() const;
    };

    // Implements buddy page allocator.
    class PageAllocator : public Singleton<PageAllocator> {
    public:
        PageAllocator();

        Optional<PageRange> allocate(usize order);

        void deallocate(PageRange range);
    };
}
