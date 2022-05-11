#include <Tests/TestSuite.hpp>

#include <Std/StringBuilder.hpp>

template<>
struct Std::Formatter<std::strong_ordering> {
    static void format(StringBuilder& builder, std::strong_ordering value)
    {
        if (value == std::strong_ordering::equal) {
            builder.append("equal");
        } else if (value == std::strong_ordering::less) {
            builder.append("less");
        } else if (value == std::strong_ordering::greater) {
            builder.append("greater");
        } else if (value == std::strong_ordering::equivalent) {
            builder.append("equivalent");
        } else {
            VERIFY_NOT_REACHED();
        }
    }
};

TEST_CASE(stringbuilder)
{
    Std::StringBuilder builder;
    builder.append("foo");
    builder.append(' ');
    builder.append("bar");
    builder.append(' ');
    builder.appendf("b{}z", "a");

    ASSERT(string_1.size() == string_1.view().size());

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
