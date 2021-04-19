#include <Kernel/GlobalMemoryAllocator.hpp>

extern "C" u8 __end__[];
extern "C" u8 __HeapLimit[];

namespace Kernel
{
    GlobalMemoryAllocator::GlobalMemoryAllocator()
        : MemoryAllocator({ __end__, __HeapLimit - __end__ - sizeof(MemoryAllocator::Node) })
    {
    }
}

extern "C"
void* malloc(usize size)
{
    return Kernel::GlobalMemoryAllocator::the().allocate(size);
}

extern "C"
void* calloc(usize nmembers, usize size)
{
    u8 *pointer = Kernel::GlobalMemoryAllocator::the().allocate(nmembers * size);
    __builtin_memset(pointer, 0, nmembers * size);
    return pointer;
}

extern "C"
void free(void *pointer)
{
    return Kernel::GlobalMemoryAllocator::the().deallocate(reinterpret_cast<u8*>(pointer));
}

extern "C"
void* realloc(void *pointer, usize size)
{
    return Kernel::GlobalMemoryAllocator::the().reallocate(reinterpret_cast<u8*>(pointer), size);
}

extern "C"
void* reallocarray(void *pointer, usize nmembers, usize size)
{
    return Kernel::GlobalMemoryAllocator::the().reallocate(reinterpret_cast<u8*>(pointer), nmembers * size);
}
