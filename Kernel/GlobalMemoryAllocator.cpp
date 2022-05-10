#include <Kernel/GlobalMemoryAllocator.hpp>
#include <Kernel/PageAllocator.hpp>
#include <Kernel/KernelMutex.hpp>
#include <Kernel/HandlerMode.hpp>

namespace Kernel
{
    static KernelMutex malloc_mutex;

    GlobalMemoryAllocator::GlobalMemoryAllocator()
        : MemoryAllocator(allocate_heap())
    {
    }

    Bytes GlobalMemoryAllocator::allocate_heap()
    {
        m_heap = PageAllocator::the().allocate(power_of_two(0x4000)).must();
        return m_heap->bytes();
    }

    u8* GlobalMemoryAllocator::allocate(usize size, bool debug_override, void *address)
    {
        VERIFY(Kernel::is_executing_in_thread_mode());

        bool were_interrupts_enabled = disable_interrupts();
        malloc_mutex.lock();

        u8 *retval = MemoryAllocator::allocate(size, debug_override, address);

        malloc_mutex.unlock();
        restore_interrupts(were_interrupts_enabled);

        return retval;
    }

    void GlobalMemoryAllocator::deallocate(u8 *pointer, bool debug_override, void *address)
    {
        bool were_interrupts_enabled = disable_interrupts();
        malloc_mutex.lock();

        MemoryAllocator::deallocate(pointer, debug_override, address);

        malloc_mutex.unlock();
        restore_interrupts(were_interrupts_enabled);
    }

    u8* GlobalMemoryAllocator::reallocate(u8 *pointer, usize size, bool debug_override, void *address)
    {
        bool were_interrupts_enabled = disable_interrupts();
        malloc_mutex.lock();

        u8 *retval = MemoryAllocator::reallocate(pointer, size, debug_override, address);

        malloc_mutex.unlock();
        restore_interrupts(were_interrupts_enabled);

        return retval;
    }
}

void* operator new(usize size)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().allocate(size, true, address);
}
void* operator new[](usize size)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().allocate(size, true, address);
}
void operator delete(void* pointer)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().deallocate(reinterpret_cast<u8*>(pointer), true, address);
}
void operator delete[](void* pointer)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().deallocate(reinterpret_cast<u8*>(pointer), true, address);
}
void operator delete(void* pointer, usize)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().deallocate(reinterpret_cast<u8*>(pointer), true, address);
}
void operator delete[](void* pointer, usize)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().deallocate(reinterpret_cast<u8*>(pointer), true, address);
}

extern "C"
void* malloc(usize size)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().allocate(size, true, address);
}

extern "C"
void* calloc(usize nmembers, usize size)
{
    void *address = __builtin_return_address(0);
    u8 *pointer = Kernel::GlobalMemoryAllocator::the().allocate(nmembers * size, true, address);

    __builtin_memset(pointer, 0, nmembers * size);

    return pointer;
}

extern "C"
void free(void *pointer)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().deallocate(reinterpret_cast<u8*>(pointer), true, address);
}

extern "C"
void* realloc(void *pointer, usize size)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().reallocate(reinterpret_cast<u8*>(pointer), size, true, address);
}

extern "C"
void* reallocarray(void *pointer, usize nmembers, usize size)
{
    void *address = __builtin_return_address(0);
    return Kernel::GlobalMemoryAllocator::the().reallocate(reinterpret_cast<u8*>(pointer), nmembers * size, true, address);
}
