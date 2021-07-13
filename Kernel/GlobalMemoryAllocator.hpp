#pragma once

#include <Std/Singleton.hpp>
#include <Std/MemoryAllocator.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/PageAllocator.hpp>

namespace Kernel
{
    class GlobalMemoryAllocator
        : public Singleton<GlobalMemoryAllocator>
        , public MemoryAllocator
    {
    private:
        friend Singleton<GlobalMemoryAllocator>;
        GlobalMemoryAllocator();

        Optional<OwnedPageRange> m_heap;

        Bytes allocate_heap();
    };
}
