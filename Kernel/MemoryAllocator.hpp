#pragma once

#include <Std/Singleton.hpp>
#include <Std/Assert.hpp>
#include <Kernel/DebugDevice.hpp>

namespace Kernel {
    using namespace Std;

    class MemoryAllocator : public Singleton<MemoryAllocator> {
    public:
        void* allocate(usize count)
        {
            TODO();
        }

        void deallocate(void*)
        {
            TODO();
        }
    };
}
