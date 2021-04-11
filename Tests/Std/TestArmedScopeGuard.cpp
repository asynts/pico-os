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

TEST_MAIN();
