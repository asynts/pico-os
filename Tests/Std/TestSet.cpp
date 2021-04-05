#include <Tests/TestSuite.hpp>

#include <Std/Set.hpp>

TEST_CASE(sortedset)
{
    Std::SortedSet<int> set;

    set.insert(3);
    set.insert(42);
    set.insert(13);
    set.insert(3);
    set.insert(-3);

    ASSERT(set.search(13) != nullptr);
    ASSERT(*set.search(13) == 13);
    ASSERT(set.size() == 4);
}

TEST_CASE(sortedset_tree)
{
    Std::SortedSet<int> set;

    set.insert(1);
    set.insert(3);
    set.insert(2);
    set.insert(7);
    set.insert(4);
    set.insert(9);
    set.insert(15);
    set.insert(4);

    ASSERT(Std::String::format("%", set) == "(nil 0x00000001 (0x00000002 0x00000003 (0x00000004 0x00000007 (nil 0x00000009 0x0000000f))))");
}

TEST_CASE(sortedset_remove_1)
{
    Std::SortedSet<int> set;

    set.insert(3);
    set.insert(5);
    set.insert(4);
    set.insert(5);
    set.insert(7);

    ASSERT(set.size() == 4);
    ASSERT(set.search(9) == nullptr);
    ASSERT(set.search(5) != nullptr);

    set.remove(5);

    ASSERT(set.size() == 3);
    ASSERT(set.search(5) == nullptr);
    ASSERT(set.search(4) != nullptr);

    set.remove(4);

    ASSERT(set.size() == 2);
    ASSERT(set.search(7) != nullptr);
    ASSERT(set.search(4) == nullptr);
}

TEST_CASE(sortedset_remove_2)
{
    Std::SortedSet<int> set;

    set.insert(7);
    set.insert(4);
    set.insert(11);
    set.insert(9);
    set.insert(8);
    set.insert(10);
    set.insert(13);

    ASSERT(Std::String::format("%", set) == "(0x00000004 0x00000007 ((0x00000008 0x00000009 0x0000000a) 0x0000000b 0x0000000d))");

    set.remove(11);

    ASSERT(Std::String::format("%", set) == "(0x00000004 0x00000007 ((0x00000008 0x00000009 0x0000000a) 0x0000000d nil))");
}

TEST_CASE(sortedset_remove_3)
{
    Std::SortedSet<int> set;

    set.insert(1);
    set.insert(2);
    set.insert(3);

    ASSERT(Std::String::format("%", set) == "(nil 0x00000001 (nil 0x00000002 0x00000003))");

    set.remove(2);

    ASSERT(Std::String::format("%", set) == "(nil 0x00000001 0x00000003)");
}

// FIXME: Test min

// FIXME: Add more tests for remove operation

// FIXME: Test weird types

TEST_MAIN();
