#pragma once

#include <Std/Singleton.hpp>
#include <Std/MemoryAllocator.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/PageAllocator.hpp>

namespace Kernel
{
    class GlobalMemoryAllocator final
        : public Singleton<GlobalMemoryAllocator>
        , public MemoryAllocator
    {
    public:
        u8* allocate(usize, bool debug_override = true, void *address = nullptr) override;
        void deallocate(u8*, bool debug_override = true, void *address = nullptr) override;
        u8* reallocate(u8*, usize, bool debug_override = true, void *address = nullptr) override;

        void set_mutex_enabled(bool enabled);

    private:
        friend Singleton<GlobalMemoryAllocator>;
        GlobalMemoryAllocator();

        Optional<OwnedPageRange> m_heap;

        Bytes allocate_heap();
    };
}
