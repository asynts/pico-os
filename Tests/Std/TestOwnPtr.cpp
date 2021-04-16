#include <Tests/TestSuite.hpp>

#include <Std/OwnPtr.hpp>

#include <type_traits>

static_assert(!std::is_copy_constructible_v<Std::OwnPtr<int>>);
static_assert(!std::is_copy_assignable_v<Std::OwnPtr<int>>);

TEST_CASE(ownptr)
{
    Std::OwnPtr<int> pointer1;

    ASSERT(pointer1.ptr() == nullptr);

    pointer1 = new int { 42 };

    ASSERT(pointer1.ptr() != nullptr);
    ASSERT(*pointer1 == 42);
}

TEST_CASE(ownptr_cons)
{
    Tests::Tracker::clear();

    {
        Std::OwnPtr<Tests::Tracker> pointer1;

        Tests::Tracker::assert(0, 0, 0, 0);

        {
            auto pointer2 = Std::make<Tests::Tracker>();

            Tests::Tracker::assert(1, 0, 0, 0);

            ASSERT(pointer2.ptr() != nullptr);
            pointer1 = move(pointer2);
            ASSERT(pointer2.ptr() == nullptr);

            Tests::Tracker::assert(1, 0, 0, 0);
        }

        Tests::Tracker::assert(1, 0, 0, 0);
    }

    Tests::Tracker::assert(1, 0, 0, 1);
}

TEST_MAIN();
