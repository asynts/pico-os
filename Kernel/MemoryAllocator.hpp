#pragma once

#include <Std/Singleton.hpp>
#include <Std/Assert.hpp>

// This symbol is defined by the linker script and marks the start of the heap.
extern u8 __end__[];

namespace Kernel {
    using namespace Std;

    auto_init_mutex(memory_allocator_mutex);

    class MemoryAllocator : public Singleton<MemoryAllocator> {
    public:
        MemoryAllocator()
        {
            printf("Initializing MemoryAllocator.\n");

            // FIXME: We should be able to put the heap at __end__, but that doesn't seem to work.
            m_free_blocks = reinterpret_cast<FreeBlock*>(__end__ + 0x10000);
            m_free_blocks->size = PICO_HEAP_SIZE - sizeof(FreeBlock) - 0x10000;
            m_free_blocks->next = nullptr;
            m_free_blocks->prev = nullptr;
        }

        u8* allocate(usize count)
        {
            // Don't let the block headers overlap.
            count = max(count, sizeof(FreeBlock));

            mutex_enter_blocking(&memory_allocator_mutex);

            for (FreeBlock *block = m_free_blocks; block; block = block->next) {
                if (block->size >= count) {
                    if (block->size >= count + sizeof(FreeBlock)) {
                        FreeBlock *new_block = reinterpret_cast<FreeBlock*>(reinterpret_cast<u8*>(block->data) + count);
                        new_block->size = block->size - count;
                        new_block->next = block->next;
                        new_block->prev = block->prev;

                        block->size = count;

                        if (block->prev)
                            block->prev->next = new_block;
                        else
                            m_free_blocks = new_block;

                        if (block->next)
                            block->next->prev = new_block;

                        printf("Allocated %p(%p)\n", block, block->data);
                        mutex_exit(&memory_allocator_mutex);
                        return block->data;
                    } else {
                        if (block->prev)
                            block->prev->next = block->next;
                        else
                            m_free_blocks = block->next;

                        if (block->next)
                            block->next->prev = block->prev;

                        printf("Allocated %p(%p)\n", block, block->data);
                        mutex_exit(&memory_allocator_mutex);
                        return block->data;
                    }
                }
            }

            printf("Allocated %p\n", nullptr);
            mutex_exit(&memory_allocator_mutex);
            return nullptr;
        }

        void deallocate(u8* data)
        {
            FreeBlock *freed_block = reinterpret_cast<FreeBlock*>(data - sizeof(FreeBlock));

            mutex_enter_blocking(&memory_allocator_mutex);

            printf("Deallocating %p(%p)\n", freed_block, freed_block->data);

            FreeBlock *closest_matching_block = nullptr;
            for (FreeBlock *block = m_free_blocks; block; block = block->next) {
                printf("Looking at block=%p\n", block);

                if (block < freed_block)
                    closest_matching_block = block;

                // Join on the left.
                if (block->data + block->size == reinterpret_cast<u8*>(freed_block)) {
                    printf("Joining freed_block=%p with block=%p on the left.\n", freed_block, block);

                    block->size += sizeof(FreeBlock) + freed_block->size;

                    // Join on the right.
                    if (block->data + block->size == reinterpret_cast<u8*>(block->next)) {
                        printf("Joining freed_block=%p with block=%p on the right.\n", freed_block, block->next);

                        block->size += sizeof(FreeBlock) + block->next->size;
                        block->next = block->next->next;
                    }

                    printf("Deallocated %p(%p)\n", freed_block, freed_block->data);
                    mutex_exit(&memory_allocator_mutex);
                    return;
                }

                // Join on the right.
                if (freed_block->data + freed_block->size == reinterpret_cast<u8*>(block)) {
                    printf("Joining freed_block=%p with block=%p on the right.\n", freed_block, block);

                    freed_block->size += sizeof(FreeBlock) + block->size;

                    if (block->prev)
                        block->prev->next = freed_block;

                    if (block->next)
                        block->next->prev = freed_block;

                    freed_block->prev = block->prev;
                    freed_block->next = block->next;

                    printf("Deallocated %p(%p)\n", freed_block, freed_block->data);
                    mutex_exit(&memory_allocator_mutex);
                    return;
                }
            }

            if (closest_matching_block) {
                printf("Appending freed_block=%p after block=%p\n", freed_block, closest_matching_block);

                if (closest_matching_block->next)
                    closest_matching_block->next->prev = freed_block;

                closest_matching_block->next = freed_block;

                freed_block->prev = closest_matching_block;
                freed_block->next = closest_matching_block->next;

                printf("Deallocated %p(%p)\n", freed_block, freed_block->data);
                mutex_exit(&memory_allocator_mutex);
                return;
            } else {
                printf("No closest matching block found.\n");
                freed_block->prev = nullptr;
                freed_block->next = m_free_blocks;

                if (m_free_blocks)
                    m_free_blocks->prev = freed_block;

                m_free_blocks = freed_block;

                printf("Deallocated %p(%p)\n", freed_block, freed_block->data);
                mutex_exit(&memory_allocator_mutex);
                return;
            }
        }

    private:
        struct FreeBlock {
            usize size;

            FreeBlock *prev;
            FreeBlock *next;

            u8 data[];
        };

        FreeBlock *m_free_blocks;
    };
}

// FIXME: Get rid of the SDK and provide the new and delete operators here.
