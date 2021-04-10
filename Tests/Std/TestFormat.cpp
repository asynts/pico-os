#include <Tests/TestSuite.hpp>

#include <Std/Format.hpp>
#include <Std/StringView.hpp>

struct A {
};

TEST_CASE(format_has_formatter)
{
    ASSERT(Std::HasFormatter<int>::value);
    ASSERT(!Std::HasFormatter<A>::value);
}

TEST_CASE(format)
{
    auto test = []<typename... Parameters>(std::string_view expected, std::string_view format, const Parameters&... parameters) {
        Std::StringBuilder builder;
        builder.appendf(Std::StringView { format.data(), format.size() }, parameters...);

        ASSERT((expected == std::string_view { builder.view().data(), builder.view().size() }));
    };

    test("0x0000002a", "{}", u32(42));
    test("0x01020304", "{}", u32(0x01020304));
    test("-0x00000001", "{}", i32(-1));
    test("0x01020304aabbccdd", "{}", u64(0x01020304aabbccddULL));
    test("0xab", "{}", u8(0xab));
    test("0x12", "{}", i8(0x12));

    test("foo bar baz", "foo {} baz", "bar");
    test("abc", "a{}c", 'b');
    test(std::string_view { "ax\0yb", 5 }, "a{}b", Std::StringView { "x\0y", 3 });

    test("a0x00000020a bXYZb c-0x00000004c", "a{}a b{}b c{}c", u32(32), "XYZ", i32(-4));
}

TEST_MAIN();
