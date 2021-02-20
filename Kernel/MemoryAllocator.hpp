#pragma once

#include <Std/Singleton.hpp>

namespace Kernel {
    using namespace Std;

    class MemoryAllocator : public Singleton<MemoryAllocator> {
    public:
        MemoryAllocator()
        {
            TODO();
        }

        void* allocate(usize)
        {
            TODO();
        }

        void deallocate(void*)
        {
            TODO();
        }
    };

    inline void* kmalloc(usize count)
    {
        return MemoryAllocator::the().allocate(count);
    }
}
