#include <Tests/TestSuite.hpp>

#include <Std/Map.hpp>
#include <Std/String.hpp>

TEST_CASE(map_without_collisions)
{
    Std::Map<Std::String, int> map;

    map.append("foo", 1);
    map.append("bar", 2);
    map.append("baz", 3);

    ASSERT(map.lookup("foo").must() == 1);
    ASSERT(map.lookup("baz").must() == 3);
    ASSERT(map.lookup("bar").must() == 2);
}

TEST_CASE(map_with_collisions)
{
    Std::Map<char, int> map;

    map.append('x', 3);
    map.append('y', 4);
    map.append('x', 5);

    ASSERT(map.lookup('x').must() == 5);
    ASSERT(map.lookup('y').must() == 4);
}

TEST_CASE(map_calls_destrucors)
{
    {
        Std::Map<int, Tests::Tracker> map;

        map.append(3, {});
        map.append(5, {});

        Tests::Tracker::clear();
    }

    Tests::Tracker::assert(0, 0, 0, 2);
}
TEST_MAIN();
