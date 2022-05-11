#include <Tests/TestSuite.hpp>

#include <Std/String.hpp>

TEST_CASE(string_compare)
{
    Std::ImmutableString string_1{ "abc" };
    Std::ImmutableString string_2{ "abc" };
    Std::ImmutableString string_3{ "abcd" };
    Std::ImmutableString string_4{ "ab" };

    ASSERT(string_1 == string_2);
    ASSERT(string_1 != string_3);
    ASSERT(string_1 != string_4);
    ASSERT(string_3 != string_4);
}

TEST_CASE(string_copy)
{
    Std::ImmutableString string_1{ "abc" };
    Std::ImmutableString string_2 = string_1;
    Std::ImmutableString string_3{ "cba" };

    ASSERT(string_1 == string_2);
    ASSERT(string_1 != string_3);

    string_1 = string_3;

    ASSERT(string_1 != string_2);

    {
        Std::ImmutableString string_4 = string_1;
    }

    ASSERT(string_1 != string_2);
    ASSERT(string_1 == string_3);
}

TEST_MAIN();
