#pragma once

#include <Std/Span.hpp>

namespace Std
{
    class MemoryAllocator {
    public:
        explicit MemoryAllocator(Bytes heap);

        usize heap_size() const { return m_heap.size(); }

        u8* allocate(usize, bool debug_override = true, void *address = nullptr);
        void deallocate(u8*, bool debug_override = true, void *address = nullptr);
        u8* reallocate(u8*, usize, bool debug_override = true, void *address = nullptr);

        void dump()
        {
            dbgln("m_freelist:");
            for (auto *entry = m_freelist; entry; entry = entry->m_next)
                dbgln("  {} ({} bytes)", entry, entry->m_size);

            auto stats = statistics();

            dbgln("statistics:");
            dbgln("  m_largest_continous_block {}", stats.m_largest_continous_block);
            dbgln("  m_avaliable_memory        {}", stats.m_avaliable_memory);
        }

        struct Statistics {
            usize m_largest_continous_block;
            usize m_avaliable_memory;
        };

        Statistics statistics()
        {
            Statistics stats;

            stats.m_largest_continous_block = 0;
            stats.m_avaliable_memory = 0;

            for (auto *entry = m_freelist; entry; entry = entry->m_next) {
                stats.m_avaliable_memory += entry->m_size;

                if (entry->m_size > stats.m_largest_continous_block)
                    stats.m_largest_continous_block = entry->m_size;
            }

            return stats;
        }

        bool m_debug = false;

    protected:
        struct Node {
            usize m_size;
            Node *m_next;
            u8 m_data[];
        };
        static_assert(sizeof(Node) % 4 == 0);

        Bytes m_heap;

    private:
        Node *m_freelist;
    };
}
