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

TEST_CASE(hashmap_iterator)
{
    Std::HashMap<int, int> map;

    map.set(89, 0);
    map.set(3, 42);
    map.set(7, 6);
    map.set(3, 16);
    map.set(100, 4);

    bool did_see_pair_1 = false;
    bool did_see_pair_3 = false;
    bool did_see_pair_4 = false;
    bool did_see_pair_5 = false;

    for (auto [key, value] : map.iter()) {
        if (key == 89 && value.must() == 0)
            ASSERT(exchange(did_see_pair_1, true) == false);
        else if (key == 7 && value.must() == 6)
            ASSERT(exchange(did_see_pair_3, true) == false);
        else if (key == 3 && value.must() == 16)
            ASSERT(exchange(did_see_pair_4, true) == false);
        else if (key == 100 && value.must() == 4)
            ASSERT(exchange(did_see_pair_5, true) == false);
        else
            ASSERT_NOT_REACHED();
    }

    ASSERT(did_see_pair_1);
    ASSERT(did_see_pair_3);
    ASSERT(did_see_pair_4);
    ASSERT(did_see_pair_5);
}

TEST_MAIN();
