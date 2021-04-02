#include <Tests/TestSuite.hpp>

#include <Std/Span.hpp>

TEST_CASE(span_bytes_from)
{
    int value = 42;

    auto bytes = Std::bytes_from(value);

    ASSERT(bytes.size() == 4);
    ASSERT(bytes.data() == reinterpret_cast<u8*>(&value));
}

TEST_MAIN();
