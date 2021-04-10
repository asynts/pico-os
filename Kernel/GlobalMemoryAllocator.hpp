#pragma once

#include <Std/Singleton.hpp>
#include <Std/MemoryAllocator.hpp>

namespace Kernel
{
    using namespace Std;

    class GlobalMemoryAllocator
        : public Singleton<GlobalMemoryAllocator>
        , public MemoryAllocator
    {
    private:
        friend Singleton<GlobalMemoryAllocator>;
        GlobalMemoryAllocator();
    };
}
