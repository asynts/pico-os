#pragma once

#include <Std/Singleton.hpp>
#include <Std/Assert.hpp>

namespace Kernel {
    using namespace Std;

    auto_init_mutex(memory_allocator_mutex);

    class MemoryAllocator : public Singleton<MemoryAllocator> {
    public:
        MemoryAllocator()
        {
            free_list = reinterpret_cast<FreeBlock*>(0x10000000);
            free_list->size = 0x40000;
            free_list->next = nullptr;
            free_list->prev = nullptr;
        }

        u8* allocate(usize count)
        {
            mutex_enter_blocking(&memory_allocator_mutex);

            for (FreeBlock *block = free_list; block; block = block->next) {
                if (block->size >= count) {
                    if (block->size >= count + sizeof(FreeBlock)) {
                        FreeBlock *new_block = reinterpret_cast<FreeBlock*>(reinterpret_cast<u8*>(block) + count);

                        if (block->prev)
                            block->prev->next = new_block;

                        if (block->next)
                            block->next->prev = new_block;

                        mutex_exit(&memory_allocator_mutex);
                        return block->data;
                    } else {
                        if (block->prev)
                            block->prev->next = block->next;

                        if (block->next)
                            block->next->prev = block->prev;

                        mutex_exit(&memory_allocator_mutex);
                        return block->data;
                    }
                }
            }

            return nullptr;
        }

        void deallocate(u8* data)
        {
            FreeBlock *freed_block = reinterpret_cast<FreeBlock*>(data - sizeof(FreeBlock));

            mutex_enter_blocking(&memory_allocator_mutex);

            FreeBlock *closest_matching_block = nullptr;
            for (FreeBlock *block = free_list; block; block = block->next) {
                if (block < freed_block)
                    closest_matching_block = block;

                // Join on the left.
                if (block->data + block->size == reinterpret_cast<u8*>(freed_block)) {
                    block->size += sizeof(FreeBlock) + freed_block->size;

                    // Join on the right.
                    if (block->data + block->size == reinterpret_cast<u8*>(block->next)) {
                        block->size += sizeof(FreeBlock) + freed_block->next->size;
                        block->next = block->next->next;
                    }

                    mutex_exit(&memory_allocator_mutex);
                    return;
                }

                // Join on the right.
                if (freed_block->data + freed_block->size == reinterpret_cast<u8*>(block)) {
                    freed_block->size += sizeof(FreeBlock) + block->size;

                    if (block->prev)
                        block->prev->next = freed_block;

                    if (block->next)
                        block->next->prev = freed_block;

                    freed_block->prev = block->prev;
                    freed_block->next = block->next;

                    mutex_exit(&memory_allocator_mutex);
                    return;
                }
            }

            if (closest_matching_block) {
                if (closest_matching_block->next)
                    closest_matching_block->next->prev = freed_block;

                closest_matching_block->next = freed_block;

                freed_block->prev = closest_matching_block;
                freed_block->next = closest_matching_block->next;

                mutex_exit(&memory_allocator_mutex);
                return;
            } else {
                freed_block->prev = nullptr;
                freed_block->next = nullptr;

                free_list = freed_block;

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

        FreeBlock *free_list;
    };
}

inline void* operator new(usize count)
{
    return Kernel::MemoryAllocator::the().allocate(count);
}

inline void* operator new[](usize count)
{
    return Kernel::MemoryAllocator::the().allocate(count);
}
