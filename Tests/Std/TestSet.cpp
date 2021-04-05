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

    ASSERT(Std::String::format("%", set) == "(nil 0x00000001 ((nil 0x00000002 nil) 0x00000003 ((nil 0x00000004 nil) 0x00000007 (nil 0x00000009 (nil 0x0000000f nil)))))");
}

TEST_MAIN();
