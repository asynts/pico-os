#include <Kernel/PageAllocator.hpp>
#include <Kernel/MemoryAllocator.hpp>

namespace Kernel {
    PageBlock* PageAllocator::allocate_page_block()
    {
        return reinterpret_cast<PageBlock*>(MemoryAllocator::the().allocate(sizeof(PageBlock)));
    }
}
