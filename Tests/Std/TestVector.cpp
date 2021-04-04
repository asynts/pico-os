#include <Tests/TestSuite.hpp>

#include <Std/Vector.hpp>

TEST_CASE(vector_default)
{
    Std::Vector<int> vec;

    ASSERT(vec.size() == 0);

    vec.append(42);

    ASSERT(vec.size() == 1);
    ASSERT(vec[0] == 42);
}

TEST_CASE(vector_inline_no_move)
{
    Tests::Tracker::clear();

    Std::Vector<Tests::Tracker, 4> vec;
    vec.append({});
    vec.append({});
    vec.append({});
    vec.append({});

    Tests::Tracker::assert(4, 4, 0, 4);
}

TEST_CASE(vector_ensure_capacity_no_move)
{
    Tests::Tracker::clear();

    Std::Vector<Tests::Tracker, 0> vec;
    vec.ensure_capacity(3);

    vec.append({});
    vec.append({});
    vec.append({});

    Tests::Tracker::assert(3, 3, 0, 3);
}

TEST_CASE(vector_extend_no_move)
{
    Std::Vector<Tests::Tracker, 1> vec1;
    Std::Vector<Tests::Tracker, 0> vec2;

    vec1.append({});
    vec1.append({});

    Tests::Tracker::clear();

    vec2.extend(vec1.span());

    Tests::Tracker::assert(0, 0, 2, 0);
}

TEST_CASE(vector_inline_no_create)
{
    Tests::Tracker::clear();

    Std::Vector<Tests::Tracker, 4> vec;

    Tests::Tracker::assert(0, {}, {}, {});
}

TEST_CASE(vector_copy)
{
    Std::Vector<int> vec1;
    vec1.append(1);

    Std::Vector<int> vec2 { vec1 };
    vec2.append(2);
    vec2.append(3);

    ASSERT(vec1.size() == 1);
    ASSERT(vec2.size() == 3);

    vec1.append(1);

    ASSERT(vec1.size() == 2);
    ASSERT(vec2.size() == 3);
}

TEST_CASE(vector_move)
{
    Std::Vector<long> vec1;
    vec1.append(1);

    Std::Vector<long> vec2 { std::move(vec1) };
    vec2.append(2);

    ASSERT(vec1.size() == 0);
    ASSERT(vec2.size() == 2);
}

TEST_CASE(vector_clear)
{
    Std::Vector<Tests::Tracker> vec;

    vec.append({});
    vec.append({});
    vec.append({});

    ASSERT(vec.size() == 3);
    usize capacity = vec.capacity();

    Tests::Tracker::clear();

    vec.clear();

    Tests::Tracker::assert(0, 0, 0, 3);

    ASSERT(vec.size() == 0);
    ASSERT(vec.capacity() == capacity);
}

TEST_MAIN();
