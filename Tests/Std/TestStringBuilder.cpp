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

TEST_CASE(stringbuilder_exceed_inline_capacity)
{
    Std::StringBuilder builder;

    for (usize i = 0; i < 513; ++i)
        builder.append('a' + i % 26);

    auto string = builder.string();
    ASSERT(string.size() == 513);

    for (usize i = 0; i < 513; ++i)
        ASSERT(string.data()[i] == 'a' + i % 26);
}

TEST_MAIN();
