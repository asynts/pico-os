#include <Tests/TestSuite.hpp>

#include <Std/Optional.hpp>

TEST_CASE(optional)
{
    Std::Optional<int> opt;

    ASSERT(opt.is_valid() == false);
    ASSERT(opt.value_or(42) == 42);

    opt = 13;

    ASSERT(opt.is_valid() == true);
    ASSERT(opt.value_or(42) == 13);
    ASSERT(opt.value() == 13);
    ASSERT(opt.must() == 13);
}

TEST_MAIN();
