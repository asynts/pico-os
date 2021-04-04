#include <Tests/TestSuite.hpp>

#include <Std/SortedSet.hpp>

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

TEST_MAIN();
