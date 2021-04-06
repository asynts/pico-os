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

TEST_CASE(sortedset_remove_4)
{
    Std::SortedSet<int> set;

    set.insert(1);
    set.insert(3);
    set.insert(2);

    ASSERT(Std::String::format("%", set) == "(nil 0x00000001 (0x00000002 0x00000003 nil))");

    set.remove(3);

    ASSERT(Std::String::format("%", set) == "(nil 0x00000001 0x00000002)");
}

TEST_CASE(sortedset_remove_5)
{
    Std::SortedSet<int> set;

    set.insert(2);
    set.insert(1);
    set.insert(3);

    ASSERT(Std::String::format("%", set) == "(0x00000001 0x00000002 0x00000003)");

    set.remove(2);

    ASSERT(Std::String::format("%", set) == "(0x00000001 0x00000003 nil)");
}

TEST_CASE(sortedset_remove_6)
{
    Std::SortedSet<int> set;

    set.insert(2);
    set.insert(1);
    set.insert(3);

    ASSERT(Std::String::format("%", set) == "(0x00000001 0x00000002 0x00000003)");

    set.remove(1);

    ASSERT(Std::String::format("%", set) == "(nil 0x00000002 0x00000003)");
}

TEST_CASE(sortedset_min)
{
    Std::SortedSet<int> set;

    set.insert(1);
    set.insert(42);
    set.insert(18);
    set.insert(7);
    set.insert(3);
    set.insert(2);
    set.insert(1);
    set.insert(-3);

    ASSERT(set.size() == 7);
    ASSERT(set.min() != nullptr);
    ASSERT(*set.min() == -3);
}

struct A {
    int m_major;
    int m_minor;

    bool operator<(const A& other) const
    {
        if (m_major < other.m_major)
            return true;

        if (m_major > other.m_major)
            return false;

        return m_minor < other.m_minor;
    }
    bool operator>(const A& other) const
    {
        if (m_major > other.m_major)
            return true;

        if (m_major < other.m_major)
            return false;

        return m_minor > other.m_minor;
    }
};

template<>
struct Std::Formatter<A> {
    static void format(Std::StringBuilder& builder, const A& value)
    {
        builder.appendf("[%.%]", value.m_major, value.m_minor);
    }
};

TEST_CASE(sortedset_custom_1)
{
    Std::SortedSet<A> set;

    set.insert({ 1, 3 });
    set.insert({ 4, 6 });
    set.insert({ 1, 4 });
    set.insert({ 1, 2 });

    ASSERT(Std::String::format("%", set) == "([0x00000001.0x00000002] [0x00000001.0x00000003] ([0x00000001.0x00000004] [0x00000004.0x00000006] nil))");
}

struct B {
    int m_indicator;
    Std::StringView m_piggyback;

    bool operator<(const B& other) const
    {
        return m_indicator < other.m_indicator;
    }

    bool operator>(const B& other) const
    {
        return m_indicator > other.m_indicator;
    }
};

template<>
struct Std::Formatter<B> {
    static void format(Std::StringBuilder& builder, const B& value)
    {
        builder.appendf("[%.%]", value.m_indicator, value.m_piggyback);
    }
};

TEST_CASE(sortedset_custom_2)
{
    Std::SortedSet<B> set;

    set.insert({ 42, "foo" });
    set.insert({ 13, "bar" });
    set.insert({ -4, "x" });

    ASSERT(Std::String::format("%", set) == "(([-0x00000004.x] [0x0000000d.bar] nil) [0x0000002a.foo] nil)");

    set.insert({ 13, "baz" });

    ASSERT(Std::String::format("%", set) == "(([-0x00000004.x] [0x0000000d.baz] nil) [0x0000002a.foo] nil)");

    set.remove({ 13, "y" });

    ASSERT(Std::String::format("%", set) == "([-0x00000004.x] [0x0000002a.foo] nil)");
}

TEST_CASE(sortedset_remove_root)
{
    Std::SortedSet<int> set;

    set.insert(3);

    ASSERT(set.size() == 1);

    set.remove(3);

    ASSERT(set.size() == 0);
    ASSERT(set.search(3) == nullptr);
}

TEST_CASE(sortedset_destructor)
{
    Tests::Tracker::clear();

    {
        Std::SortedSet<Tests::Tracker> set;

        Tests::Tracker::assert(0, 0, 0, 0);

        set.insert({ 1 });
        set.insert({ 1 });

        Tests::Tracker::assert(2, 2, 0, 2);
    }

    Tests::Tracker::assert(2, 2, 0, 3);
}

TEST_CASE(sortedset_iterator)
{
    Std::SortedSet<int> set;

    set.insert(4);
    set.insert(1);
    set.insert(8);
    set.insert(2);
    set.insert(7);
    set.insert(6);
    set.insert(3);
    set.insert(5);

    auto iter = set.inorder();

    ASSERT(iter != iter.end());

    ASSERT(*iter++ == 1);
    ASSERT(*iter++ == 2);
    ASSERT(*iter++ == 3);
    ASSERT(*iter++ == 4);
    ASSERT(*iter++ == 5);
    ASSERT(*iter++ == 6);
    ASSERT(*iter++ == 7);
    ASSERT(*iter++ == 8);

    ASSERT(iter == iter.end());
}

TEST_MAIN();
