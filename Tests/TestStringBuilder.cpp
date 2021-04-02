#include <Tests/TestSuite.hpp>

#include <Std/StringBuilder.hpp>

TEST_CASE(stringbuilder)
{
    Std::StringBuilder builder;
    builder.append("foo");
    builder.append(' ');
    builder.append("bar");
    builder.append(' ');
    builder.appendf("b%z", "a");

    ASSERT(builder.view() == "foo bar baz");
    ASSERT(builder.string().view() == "foo bar baz");
}

TEST_MAIN();
