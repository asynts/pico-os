#include <Tests/TestSuite.hpp>

#include <Std/StringView.hpp>

#include <string_view>
#include <span>

TEST_CASE(stringview_trivial)
{
    ASSERT(Std::StringView{ "abc" } == Std::StringView{ "abc" });
    ASSERT(Std::StringView{ "abc" } != Std::StringView{ "abcd" });
}

TEST_CASE(stringview_cstring)
{
    Std::StringView sv { "Hello, world!" };

    ASSERT((std::string_view { "Hello, world!" } == std::string_view { sv.data(), sv.size() }));

    sv = "foo";

    ASSERT((std::string_view { "foo" } == std::string_view { sv.data(), sv.size() }));
}

TEST_CASE(stringview_index_of)
{
    Std::StringView sv = "0123456";

    ASSERT(sv.index_of('0').must() == 0);
    ASSERT(sv.index_of('3').must() == 3);
    ASSERT(sv.index_of('7').is_valid() == false);
}

TEST_CASE(stringview_equal)
{
    Std::StringView sv1 = "foo";
    Std::StringView sv2 = "bar";
    Std::StringView sv3 = sv2;
    Std::StringView sv4 = "foobar" + 3;

    ASSERT(sv1 != sv2);
    ASSERT(sv2 == sv3);
    ASSERT(sv3 == sv4);
}

TEST_CASE(stringview_substr)
{
    Std::StringView sv = "foobarbaz";
    ASSERT(sv.substr(0) == "foobarbaz");
    ASSERT(sv.substr(3) == "barbaz");
    ASSERT(sv.substr(3, 6) == "bar");
}

TEST_CASE(stringview_trim)
{
    Std::StringView sv = "xy";
    ASSERT(sv.trim(3).size() == 2);
    ASSERT(sv.trim(1) == "x");
}

TEST_CASE(stringview_strcpy_1)
{
    Std::StringView sv = "Hello, world!";
    std::array<char, 32> buffer;

    sv.strcpy_to({ buffer.data(), buffer.size() });

    auto actual = std::span<char> { buffer }.subspan(0, 14);
    std::array<char, 14> expected = { 72, 101, 108, 108, 111, 44, 32, 119, 111, 114, 108, 100, 33, 0 };
    ASSERT(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
}

TEST_CASE(stringview_strcpy_2)
{
    Std::StringView sv = { "foo\0bar", 7 };
    std::array<char, 32> buffer;

    sv.strcpy_to({ buffer.data(), buffer.size() });

    auto actual = std::span<char> { buffer }.subspan(0, 8);
    std::array<char, 8> expected = { 102, 111, 111, 0, 98, 97, 114, 0 };
    ASSERT(std::equal(actual.begin(), actual.end(), expected.begin(), expected.end()));
}

TEST_MAIN();
