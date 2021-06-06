#pragma once

#include <Std/Singleton.hpp>
#include <Std/Array.hpp>
#include <Std/Optional.hpp>
#include <Std/Format.hpp>

namespace Kernel
{
    using namespace Std;

    class PageAllocator : public Singleton<PageAllocator> {
    public:
        static constexpr usize max_power = 18;

        Optional<uptr> allocate(usize power_of_two)
        {
            usize size = 1 << power_of_two;

            dbgln("[PageAllocator::allocate] power_of_two={}", power_of_two);

            ASSERT(power_of_two <= max_power);

            if (m_blocks[power_of_two] != nullptr) {
                uptr block = reinterpret_cast<uptr>(m_blocks[power_of_two]);

                dbgln("[PageAllocator::allocate] Found suitable block {}", block);

                m_blocks[power_of_two] = m_blocks[power_of_two]->m_next;
                return block;
            }

            ASSERT(power_of_two < max_power);

            auto block_opt = allocate(power_of_two + 1);
            if (!block_opt.is_valid()) {
                return {};
            }
            uptr block1 = block_opt.value();
            Block *block2 = reinterpret_cast<Block*>(block1 + size);

            dbgln("[PageAllocator::allocate] Splitting larger block block1={} block2={}", block1, block2);

            block2->m_next = m_blocks[power_of_two];
            m_blocks[power_of_two] = block2;

            return block1;
        }

    private:
        friend Singleton<PageAllocator>;
        PageAllocator();

        // There is quite a bit of trickery going on here:
        //
        //   - The address of this block is encoded indirectly in the address of this object
        //
        //   - The size of this block is encoded indirection in the index used to access m_blocks
        struct Block {
            Block *m_next;
        };

        Array<Block*, max_power + 1> m_blocks;
    };
}
