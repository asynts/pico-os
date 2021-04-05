#include <Tests/TestSuite.hpp>

#include <Std/HashMap.hpp>

TEST_CASE(hashmap)
{
    Std::HashMap<int, int> map;

    map.set(42, 13);
    map.set(13, 51);
    map.set(-3, -3);
    map.set(42, 7);

    ASSERT(map.size() == 3);

    ASSERT(map.get(13) != nullptr && *map.get(13) == 51);
    ASSERT(map.get(16) == nullptr);
    ASSERT(map.get(42) != nullptr && *map.get(42) == 7);

    map.remove(42);

    ASSERT(map.size() == 2);

    ASSERT(map.get(-3) != nullptr && *map.get(-3) == -3);
}

TEST_MAIN();
