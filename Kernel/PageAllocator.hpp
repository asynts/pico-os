#pragma once

#include <Std/Singleton.hpp>
#include <Std/Assert.hpp>
#include <Kernel/DebugDevice.hpp>

namespace Kernel {
    using namespace Std;

    struct PageBlock {
        usize order;
        u8 *base;
        PageBlock *next;
    };

    auto_init_mutex(page_allocator_mutex);
    class PageAllocator : public Singleton<PageAllocator> {
    public:
        PageAllocator()
        {
            TODO();
        }

        PageBlock* allocate(usize order)
        {
            ASSERT(order < max_order);

            mutex_enter_blocking(&page_allocator_mutex);

            if (m_blocks[order] != nullptr) {
                PageBlock *block = m_blocks[order];
                m_blocks[order] = block->next;

                mutex_exit(&page_allocator_mutex);
                return block;
            }

            PageBlock *block = allocate_page_block();
            for(size_t i = order + 1; i < max_order; ++i) {
                if (m_blocks[i] != nullptr) {
                    block->base = m_blocks[i]->base;
                    block->next = m_blocks[i]->next;
                    block->order = m_blocks[i]->order - 1;

                    exchange(m_blocks[i], block);
                    block->order = block->order - 1;
                    block->base += 2 << block->order;

                    mutex_exit(&page_allocator_mutex);
                    return allocate(order);
                }
            }

            mutex_exit(&page_allocator_mutex);
            return nullptr;
        }

        void deallocate(PageBlock* block)
        {
            mutex_enter_blocking(&page_allocator_mutex);

            block->next = m_blocks[block->order];
            m_blocks[block->order] = block;

            mutex_exit(&page_allocator_mutex);
        }

    private:
        PageBlock* allocate_page_block();

        constexpr static usize max_order = 18 + 1;

        PageBlock *m_blocks[max_order] = {nullptr};
    };
}
