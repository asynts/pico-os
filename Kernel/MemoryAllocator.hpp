#pragma once

#include <Std/Singleton.hpp>

namespace Kernel
{
    using namespace Std;

    struct FreeListEntry {
        usize m_size;
        FreeListEntry *m_next;
        u8 m_data[];
    };
    static_assert(sizeof(FreeListEntry) % 4 == 0);

    class MemoryAllocator : public Singleton<MemoryAllocator> {
    public:
        void* allocate(usize);
        void deallocate(void*);
        void* reallocate(void*, usize);

    private:
        friend Singleton<MemoryAllocator>;
        MemoryAllocator();

        FreeListEntry *m_freelist;
    };
}
