#include <Tests/TestSuite.hpp>

#include <Std/HashTable.hpp>

TEST_CASE(hashtable_int)
{
    Std::HashTable<u32> hash;

    hash.insert(1);
    hash.insert(2);
    hash.insert(3);
    hash.insert(4);
    hash.insert(5);
    hash.insert(6);
    hash.insert(7);
    hash.insert(8);
    hash.insert(9);

    hash.insert(6);

    ASSERT(hash.size() == 9);
}

struct A {
    int m_hash;
    int m_value;

    bool operator<(const A& other) const
    {
        return m_value < other.m_value;
    }
    bool operator>(const A& other) const
    {
        return m_value > other.m_value;
    }

    u32 hash() const
    {
        return m_hash;
    }
};

template<>
struct Std::Hash<A> {
    static u32 compute(A value)
    {
        return value.m_hash;
    }
};

TEST_CASE(hashtable_collisions)
{
    Std::HashTable<A> hash;

    hash.insert({ 1, 42 });
    hash.insert({ 2, 13 });
    hash.insert({ 3, -7 });
    hash.insert({ 2, -11 });

    ASSERT(hash.size() == 4);

    hash.remove({ 2, -11 });

    ASSERT(hash.size() == 3);

    ASSERT(hash.search({ 2, 13 }) != nullptr);
    ASSERT(hash.search({ 2, 13 })->m_value == 13);

    ASSERT(hash.search({ 2, -11 }) == nullptr);
    ASSERT(hash.search({ 3, 7 }) == nullptr);
}

TEST_CASE(hashtable_iterator)
{
    Std::HashTable<int> hash;

    hash.insert(14);
    hash.insert(72);
    hash.insert(3);
    hash.insert(0);

    bool did_see_14 = false;
    bool did_see_72 = false;
    bool did_see_3 = false;
    bool did_see_0 = false;

    for (int value : hash.iter()) {
        if (value == 14)
            ASSERT(std::exchange(did_see_14, true) == false);
        else if (value == 72)
            ASSERT(std::exchange(did_see_72, true) == false);
        else if (value == 3)
            ASSERT(std::exchange(did_see_3, true) == false);
        else if (value == 0)
            ASSERT(std::exchange(did_see_0, true) == false);
        else
            ASSERT_NOT_REACHED();
    }

    ASSERT(did_see_14);
    ASSERT(did_see_72);
    ASSERT(did_see_3);
    ASSERT(did_see_0);
}

TEST_MAIN();
