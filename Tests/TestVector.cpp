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

struct A
{
    static size_t create_count;
    static size_t copy_count;
    static size_t move_count;

    A()
    {
        create_count++;
    }

    A(const A&)
    {
        copy_count++;
    }
    A(A&&)
    {
        move_count++;
    }
};

size_t A::create_count;
size_t A::copy_count;
size_t A::move_count;

TEST_CASE(vector_inline_no_move)
{
    A::create_count = 0;
    A::copy_count = 0;
    A::move_count = 0;

    Std::Vector<A, 4> vec;
    vec.append({});
    vec.append({});
    vec.append({});
    vec.append({});

    ASSERT(A::create_count == 4);
    ASSERT(A::move_count == 4);
    ASSERT(A::copy_count == 0);
}

TEST_CASE(vector_ensure_capacity_no_move)
{
    A::create_count = 0;
    A::copy_count = 0;
    A::move_count = 0;

    Std::Vector<A, 0> vec;
    vec.ensure_capacity(3);

    vec.append({});
    vec.append({});
    vec.append({});

    ASSERT(A::create_count == 3);
    ASSERT(A::move_count == 3);
    ASSERT(A::copy_count == 0);
}

TEST_CASE(vector_extend_no_move)
{
    Std::Vector<A, 1> vec1;
    Std::Vector<A, 0> vec2;

    vec1.append({});
    vec1.append({});

    A::create_count = 0;
    A::copy_count = 0;
    A::move_count = 0;

    vec2.extend(vec1.span());

    ASSERT(A::create_count == 0);
    ASSERT(A::move_count == 0);
    ASSERT(A::copy_count == 2);
}

TEST_CASE(vector_inline_no_create)
{
    A::create_count = 0;
    A::copy_count = 0;
    A::move_count = 0;

    Std::Vector<A, 4> vec;

    ASSERT(A::create_count == 0);
    ASSERT(A::move_count == 0);
    ASSERT(A::copy_count == 0);
}

TEST_MAIN();
