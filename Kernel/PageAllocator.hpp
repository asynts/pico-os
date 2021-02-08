#pragma once

namespace Kernel {
    class PageRange {
    public:
        PageRange(flatptr_t base, size_t order);

        size_t order() const;
        size_t size() const;
        size_t pages() const;
    };

    class PageAllocator : public Singleton<PageAllocator> {
    public:
        PageAllocator();

        Optional<PageRange> allocate(size_t order);

        void deallocate(PageRange range);
    };
}
