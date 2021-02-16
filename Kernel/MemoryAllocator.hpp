#pragma once

namespace Kernel {
    // Implements slab allocator with first fit.
    class MemoryAllocator : public Singleton<MemoryAllocator> {
    public:
        MemoryAllocator();

        flatptr_t allocate(usize);

        void deallocate(flatptr_t);
    };
}
