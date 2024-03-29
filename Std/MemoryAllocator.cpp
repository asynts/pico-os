#include <Std/MemoryAllocator.hpp>
#include <Std/Format.hpp>

namespace Std
{
    static usize round_to_word(usize size)
    {
        if (size % 4 != 0)
            size = size + 4 - size % 4;
        return size;
    }

    MemoryAllocator::MemoryAllocator(Bytes heap)
        : m_heap(heap)
    {
        m_freelist = reinterpret_cast<Node*>(heap.data());
        m_freelist->m_size = heap.size() - sizeof(Node);
        m_freelist->m_next = nullptr;
    }

    u8* MemoryAllocator::allocate(usize size, bool debug_override, void *address)
    {
        if (address == nullptr)
            address = __builtin_return_address(0);

        Node *previous = nullptr;
        for (Node *entry = m_freelist; entry; entry = entry->m_next)
        {
            if (entry->m_size >= size + sizeof(Node))
            {
                auto *next = reinterpret_cast<Node*>(entry->m_data + round_to_word(size));
                next->m_next = entry->m_next;
                next->m_size = entry->m_size - round_to_word(size) - sizeof(Node);

                if (previous)
                    previous->m_next = next;
                else
                    m_freelist = next;

                entry->m_next = nullptr;
                entry->m_size = round_to_word(size);

                VERIFY(usize(entry->m_data) % 4 == 0);

                if (m_debug && debug_override)
                    dbgln("\e[32mMTRACE: @ {} + {} {}\e[0m", address, entry->m_data, size);

                return entry->m_data;
            }

            previous = entry;
        }

        dump();

        VERIFY_NOT_REACHED();
    }

    void MemoryAllocator::deallocate(u8 *pointer, bool debug_override, void *address)
    {
        if (pointer == nullptr)
            return;

        if (address == nullptr)
            address = __builtin_return_address(0);

        if (m_debug && debug_override)
            dbgln("\e[32mMTRACE: @ {} - {}", address, pointer);

        auto *target_entry = reinterpret_cast<Node*>((u8*)pointer - sizeof(Node));

        Node *previous = nullptr;
        for (auto *entry = m_freelist; entry; entry = entry->m_next)
        {
            if (entry > target_entry)
            {
                // Try to merge on the left
                if (previous && previous->m_data + previous->m_size == (u8*)target_entry) {
                    previous->m_size += sizeof(Node) + target_entry->m_size;

                    // Additionally, try to merge on the right
                    if (previous->m_data + previous->m_size == (u8*)entry) {
                        previous->m_size += sizeof(Node) + entry->m_size;
                        previous->m_next = entry->m_next;
                    }

                    return;
                }

                // Try to merge on the right
                if (target_entry->m_data + target_entry->m_size == (u8*)entry) {
                    target_entry->m_size += sizeof(Node) + entry->m_size;
                    target_entry->m_next = entry->m_next;
                } else {
                    target_entry->m_next = entry;
                }

                if (previous)
                    previous->m_next = target_entry;
                else
                    m_freelist = target_entry;

                return;
            }

            previous = entry;
        }

        m_freelist = target_entry;
    }

    u8* MemoryAllocator::reallocate(u8 *pointer, usize size, bool debug_override, void *address)
    {
        if (address == nullptr)
            address = __builtin_return_address(0);

        if (m_debug && debug_override)
            dbgln("\e[32mMTRACE: @ {} < {}\e[0m", address, pointer);

        auto *entry = reinterpret_cast<Node*>((u8*)pointer - sizeof(Node));

        if (size < entry->m_size) {
            if (m_debug && debug_override)
                dbgln("\e[32mMTRACE: @ {} > {} {}\e[0m", address, pointer, size);

            return pointer;
        }

        u8 *new_pointer = allocate(size, false);
        memcpy(new_pointer, pointer, entry->m_size);
        deallocate(pointer, false);

        dbgln("\e[32mMTRACE: @ {} > {} {}\e[0m", address, new_pointer, size);

        return new_pointer;
    }
}
