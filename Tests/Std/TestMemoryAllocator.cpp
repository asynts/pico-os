#include <Tests/TestSuite.hpp>

#include <Std/MemoryAllocator.hpp>

#include <cstring>
#include <vector>
#include <algorithm>
#include <functional>
#include <random>

// We are making some assumptions here about the nature of the implementation

TEST_CASE(memoryallocator)
{
    std::array<uint8_t, 0x200> heap;

    Std::MemoryAllocator mem { { heap.data(), heap.size() } };

    u8 *pointer1 = mem.allocate(32);
    u8 *pointer2 = mem.allocate(64);
    u8 *pointer3 = mem.allocate(32);

    mem.deallocate(pointer2);

    u8 *pointer4 = mem.allocate(64);

    std::memset(pointer4, 0x33, 64);

    std::memset(pointer1, 0xff, 32);
    std::memset(pointer3, 0xff, 32);

    for (usize i = 0; i < 64; ++i)
        ASSERT(pointer4[i] == 0x33);
}

TEST_CASE(memoryallocator_death_by_a_thousand_cuts)
{
    std::array<uint8_t, 0xc0000> heap;

    constexpr usize max_allocations = (heap.size() / 20) - 1;

    Std::MemoryAllocator mem { { heap.data(), heap.size() } };

    auto stats_before = mem.statistics();

    ASSERT(stats_before.m_largest_continous_block > heap.size() / 2);

    std::vector<u8*> allocations;
    for (usize i = 0; i < max_allocations; ++i)
        allocations.push_back(mem.allocate(4));

    auto stats_middle = mem.statistics();
    ASSERT(stats_middle.m_largest_continous_block < heap.size() / 4);

    // We want to deallocate in a random, but reproducible order
    std::mt19937 prng { 3040088493306752707 };
    std::shuffle(allocations.begin(), allocations.end(), prng);

    for (u8 *allocation : allocations)
        mem.deallocate(allocation);

    auto stats_after = mem.statistics();

    ASSERT(stats_before.m_largest_continous_block == stats_after.m_largest_continous_block);
}

TEST_MAIN();
