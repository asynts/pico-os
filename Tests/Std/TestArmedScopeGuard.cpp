#include <Tests/TestSuite.hpp>

#include <Std/ArmedScopeGuard.hpp>

TEST_CASE(armedscopeguard)
{
    int i = 0;

    {
        Std::ArmedScopeGuard guard = [&] {
            ++i;
        };

        ASSERT(i == 0);
        i = 42;

        guard.disarm();
    }

    ASSERT(i == 42);
    i = 13;

    {
        Std::ArmedScopeGuard guard = [&] {
            ++i;
        };

        ASSERT(i == 13);
    }

    ASSERT(i == 14);
}

TEST_CASE(armedscopeguard_move)
{
    int i = 0;

    {
        Std::ArmedScopeGuard guard1 = [&] {
            ++i;
        };

        ASSERT(i == 0);

        {
            Std::ArmedScopeGuard guard2 = move(guard1);

            ASSERT(i == 0);

            Std::ArmedScopeGuard guard3 = move(guard2);

            ASSERT(i == 0);
        }

        ASSERT(i == 1);
    }

    ASSERT(i == 1);
}

TEST_MAIN();
