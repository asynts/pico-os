#include <Tests/TestSuite.hpp>

#include <Std/RefPtr.hpp>

struct A : Std::RefCounted<A> {
};

TEST_CASE(refptr)
{
    Std::RefPtr<A> refptr = A::construct();
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

TEST_MAIN();
