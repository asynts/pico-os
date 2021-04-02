#include <Tests/TestSuite.hpp>

#include <Std/Path.hpp>

TEST_CASE(path)
{
    Std::Path path { "/foo/bar/baz" };
    ASSERT(path.is_absolute());
    ASSERT(path.components().size() == 3);
    ASSERT(path.components()[0] == "foo");
    ASSERT(path.components()[1] == "bar");
    ASSERT(path.components()[2] == "baz");
    ASSERT(path.components()[0] != "xyz");

    ASSERT(path.string() == "/foo/bar/baz");

    // FIXME: Using the '/' operator seems to free the underlying strings when the new object goes out of scope?
    ASSERT((path / "xyz").string() == "/foo/bar/baz/xyz");

    ASSERT(path.string() == "/foo/bar/baz");

    ASSERT((path / "x").is_absolute());

    ASSERT(!Std::Path { "x" }.is_absolute());
}

TEST_MAIN();
