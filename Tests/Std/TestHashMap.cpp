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

TEST_CASE(hashmap_strings)
{
    Std::HashMap<Std::String, Std::String> map;

    map.set("/redirect/foo", "/etc/foo");
    map.set("/redirect/bar", "/proc/mounts");
    map.set("/redirect/baz", "/bin/Shell.elf");

    map.set("/redirect/bar", "/proc/mounts2");

    ASSERT(map.size() == 3);
    ASSERT(map.get("/redirect/bar") != nullptr);
    ASSERT(*map.get("/redirect/bar") == "/proc/mounts2");
}

TEST_MAIN();
