#include <Kernel/KernelMutex.hpp>

namespace Kernel
{
    KernelMutex dbgln_mutex;
    KernelMutex malloc_mutex;
    KernelMutex page_allocator_mutex;
}
