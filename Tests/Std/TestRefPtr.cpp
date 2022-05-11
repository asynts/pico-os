#include <Tests/TestSuite.hpp>

#include <Std/RefPtr.hpp>

struct A : Std::RefCounted<A> {
};

TEST_CASE(refptr)
{
    Std::RefPtr<A> refptr = A::construct();
    ASSERT(refptr->refcount() == 1);
}

struct B : Tests::Tracker, Std::RefCounted<B> {
    B() : Tests::Tracker() { }
};

TEST_CASE(refptr_destructor_called)
{
    Tests::Tracker::clear();

    {
        auto refptr = B::construct();

        Tests::Tracker::assert(1, 0, 0, 0);
    }

    Tests::Tracker::assert(1, 0, 0, 1);
}

TEST_CASE(refptr_rawptr)
{
    Tests::Tracker::clear();

    B *rawptr;
    {
        auto refptr = B::construct();

        rawptr = refptr;
        rawptr->ref();

        Tests::Tracker::assert(1, 0, 0, 0);
    }

    Tests::Tracker::assert(1, 0, 0, 0);

    rawptr->unref();

    Tests::Tracker::assert(1, 0, 0, 1);
}

TEST_CASE(refptr_multiple)
{
    Tests::Tracker::clear();

    {
        auto refptr1 = B::construct();

        {
            Std::RefPtr<B> refptr2 = refptr1;

            Tests::Tracker::assert(1, 0, 0, 0);
        }

        Tests::Tracker::assert(1, 0, 0, 0);
    }

    Tests::Tracker::assert(1, 0, 0, 1);
}

TEST_CASE(refptr_nullptr)
{
    Tests::Tracker::clear();

    {
        Std::RefPtr<B> refptr;
    }

    Tests::Tracker::assert(0, 0, 0, 0);
}

TEST_CASE(refptr_move)
{
    Tests::Tracker::clear();

    auto refptr1 = B::construct();
    auto refptr2 = move(refptr1);

    VERIFY(refptr1 == nullptr);
    VERIFY(refptr2->refcount() == 1);
}

TEST_CASE(refptr_assign)
{
    Tests::Tracker::clear();

    auto refptr1 = B::construct();
    auto refptr2 = B::construct();

    Tests::Tracker::assert(2, 0, 0, 0);

    refptr2 = refptr1;

    Tests::Tracker::assert(2, 0, 0, 1);
}

static void foo(Std::RefPtr<B>) {
    // Let parameter go out of scope.
}

TEST_CASE(refptr_move_into_function)
{
    Tests::Tracker::clear();

    {
        auto refptr_1 = B::construct();

        Tests::Tracker::assert(1, 0, 0, 0);

        foo(refptr_1);

        Tests::Tracker::assert(1, 0, 0, 0);

        foo(std::move(refptr_1));

        Tests::Tracker::assert(1, 0, 0, 1);
    }

    Tests::Tracker::assert(1, 0, 0, 1);
}

static Std::RefPtr<B> bar() {
    return B::construct();
}

TEST_CASE(refptr_return_from_function)
{
    Tests::Tracker::clear();

    {
        auto refptr_1 = bar();

        Tests::Tracker::assert(1, 0, 0, 0);
    }

    Tests::Tracker::assert(1, 0, 0, 1);

    {
        bar();

        Tests::Tracker::assert(2, 0, 0, 2);
    }
}

TEST_MAIN();
