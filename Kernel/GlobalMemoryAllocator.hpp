#pragma once

#include <Std/Singleton.hpp>
#include <Std/MemoryAllocator.hpp>

#include <Kernel/Forward.hpp>

namespace Kernel
{
    class GlobalMemoryAllocator
        : public Singleton<GlobalMemoryAllocator>
        , public MemoryAllocator
    {
    private:
        friend Singleton<GlobalMemoryAllocator>;
        GlobalMemoryAllocator();
    };
}
