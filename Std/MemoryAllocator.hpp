#pragma once

#include <Std/Span.hpp>

namespace Std
{
    class MemoryAllocator {
    public:
        explicit MemoryAllocator(Bytes heap);

        usize heap_size() const { return m_heap.size(); }

        u8* allocate(usize);
        void deallocate(u8*);
        u8* reallocate(u8*, usize);

        void dump()
        {
            dbgln("m_freelist:");
            for (auto *entry = m_freelist; entry; entry = entry->m_next)
                dbgln("  % (% bytes)", entry, entry->m_size);
        }

        struct Statistics {
            usize m_largest_continous_block;
        };

        Statistics statistics()
        {
            Statistics stats;

            stats.m_largest_continous_block = 0;

            for (auto *entry = m_freelist; entry; entry = entry->m_next) {
                if (entry->m_size > stats.m_largest_continous_block)
                    stats.m_largest_continous_block = entry->m_size;
            }

            return stats;
        }

    protected:
        struct Node {
            usize m_size;
            Node *m_next;
            u8 m_data[];
        };
        static_assert(sizeof(Node) % 4 == 0);

    private:
        Node *m_freelist;
        Bytes m_heap;
    };
}
