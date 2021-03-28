#include <Kernel/MemoryAllocator.hpp>

#include <pico/printf.h>

extern "C" u8 __end__[];
extern "C" u8 __HeapLimit[];

namespace Kernel
{
    static usize round_to_word(usize size)
    {
        if (size % 4 != 0)
            size = size + 4 - size % 4;
        return size;
    }

    MemoryAllocator::MemoryAllocator()
    {
        m_freelist = reinterpret_cast<FreeListEntry*>(__end__);
        m_freelist->m_size = __HeapLimit - __end__ - sizeof(FreeListEntry);
        m_freelist->m_next = nullptr;
    }

    void* MemoryAllocator::allocate(usize size)
    {
        FreeListEntry *previous = nullptr;
        for (FreeListEntry *entry = m_freelist; entry; entry = entry->m_next)
        {
            if (entry->m_size >= size + sizeof(FreeListEntry))
            {
                auto *next = reinterpret_cast<FreeListEntry*>(entry->m_data + round_to_word(size));
                next->m_next = entry->m_next;
                next->m_size = entry->m_size - round_to_word(size) - sizeof(FreeListEntry);

                if (previous)
                    previous->m_next = next;
                else
                    m_freelist = next;

                entry->m_next = nullptr;
                entry->m_size = round_to_word(size);

                return entry->m_data;
            }

            previous = entry;
        }

        VERIFY_NOT_REACHED();
    }
    void MemoryAllocator::deallocate(void *pointer)
    {
        if (pointer == nullptr)
            return;

        auto *target_entry = reinterpret_cast<FreeListEntry*>((u8*)pointer - sizeof(FreeListEntry));

        FreeListEntry *previous = nullptr;
        for (auto *entry = m_freelist; entry; entry = entry->m_next)
        {
            if (entry > target_entry)
            {
                // Try to merge on the left
                if (previous && previous->m_data + previous->m_size == (u8*)target_entry) {
                    previous->m_size += sizeof(FreeListEntry) + target_entry->m_size;

                    // Additionally, try to merge on the right
                    if (previous->m_data + previous->m_size == (u8*)entry) {
                        previous->m_size += sizeof(FreeListEntry) + entry->m_size;
                        previous->m_next = entry->m_next;
                    }

                    return;
                }

                // Try to merge on the right
                if (target_entry->m_data + target_entry->m_size == (u8*)entry) {
                    target_entry->m_size += sizeof(FreeListEntry) + entry->m_size;
                    target_entry->m_next = entry->m_next;
                } else {
                    target_entry->m_next = entry;
                }

                if (previous)
                    previous->m_next = target_entry;
                else
                    m_freelist = target_entry;
            }

            previous = entry;
        }

        m_freelist = target_entry;
    }
    void* MemoryAllocator::reallocate(void *pointer, usize size)
    {
        auto *entry = reinterpret_cast<FreeListEntry*>((u8*)pointer - sizeof(FreeListEntry));

        if (size < entry->m_size)
            return pointer;

        void *new_pointer = allocate(size);
        memcpy(new_pointer, pointer, entry->m_size);
        deallocate(pointer);
        return new_pointer;
    }
}

extern "C"
void* malloc(usize size)
{
    return Kernel::MemoryAllocator::the().allocate(size);
}

extern "C"
void free(void *pointer)
{
    return Kernel::MemoryAllocator::the().deallocate(pointer);
}

extern "C"
void* realloc(void *pointer, usize size)
{
    return Kernel::MemoryAllocator::the().reallocate(pointer, size);
}
