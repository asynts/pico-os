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

TEST_CASE(path_parent_1)
{
    Std::Path path1 { "x/y/z/w" };

    ASSERT(path1.string() == "x/y/z/w");
    ASSERT(path1.is_absolute() == false);

    Std::Path path2 = path1.parent();
    ASSERT(path2.string() == "x/y/z");
    ASSERT(path2.is_absolute() == false);

    Std::Path path3 = path2.parent();
    ASSERT(path3.string() == "x/y");
    ASSERT(path3.is_absolute() == false);

    Std::Path path4 = path3.parent();
    ASSERT(path4.string() == "x");
    ASSERT(path4.is_absolute() == false);
}

TEST_CASE(path_parent_2)
{
    Std::Path path1 { "/x/y" };

    ASSERT(path1.string() == "/x/y");
    ASSERT(path1.is_absolute() == true);

    Std::Path path2 = path1.parent();
    ASSERT(path2.string() == "/x");
    ASSERT(path2.is_absolute() == true);

    path1 = path1 / "z";

    ASSERT(path1.string() == "/x/y/z");
    ASSERT(path1.is_absolute() == true);

    ASSERT(path2.string() == "/x");
    ASSERT(path2.string() != "/y");
    ASSERT(path2.is_absolute() == true);
}

TEST_CASE(path_filename)
{
    Std::Path path1 { "/x/y" };

    ASSERT(path1.filename() == "y");
    ASSERT(path1.parent().filename() == "x");
}

TEST_CASE(path_absolute_regression)
{
    Std::Path path1 { "/foo.txt" };

    ASSERT(path1.is_absolute() == true);

    Std::Path path2 = path1.parent();
    ASSERT(path2.string() == "/");
    ASSERT(path2.is_absolute() == true);
}

TEST_MAIN();
