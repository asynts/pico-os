#include <Tests/TestSuite.hpp>

#include <array>

#include <Std/Span.hpp>

TEST_CASE(span_bytes_from)
{
    int value = 42;

    auto bytes = Std::bytes_from(value);

    ASSERT(bytes.size() == 4);
    ASSERT(bytes.data() == reinterpret_cast<u8*>(&value));
}

TEST_CASE(span_copy_to_exact)
{
    int value1 = 42;
    auto bytes1 = Std::bytes_from(value1);

    int value2 = 0;
    auto bytes2 = Std::bytes_from(value2);

    bytes1.copy_to(bytes2);
    ASSERT(value1 == 42 && value2 == 42);

    value2 = 13;
    bytes2.copy_trimmed_to(bytes1);
    ASSERT(value1 == 13 && value2 == 13);
}

TEST_CASE(span_slice)
{
    std::array<u8, 64> buffer;
    auto bytes = Std::Bytes { buffer.data(), buffer.size() };

    bytes.slice(3);
    ASSERT(bytes.data() == buffer.data());
    ASSERT(bytes.size() == buffer.size());

    ASSERT(bytes.slice(3).data() == buffer.data() + 3);
    ASSERT(bytes.slice(5).size() == buffer.size() - 5);

    ASSERT(bytes.slice(buffer.size()).data() == buffer.data() + buffer.size());
    ASSERT(bytes.slice(buffer.size()).size() == 0);
}

TEST_CASE(span_iterator)
{
    std::array<int, 8> buffer { 3, 0, 1, 8, 1, 2, 4, 10 };
    auto span = Std::Span<int> { buffer.data(), buffer.size() };

    auto iter = span.iter();

    ASSERT(*iter == 3);
    iter++;
    ASSERT(*iter == 0);

    ASSERT(iter.size() == 7);
    ASSERT(iter.data() == buffer.data() + 1);
    ASSERT((iter.end() == Std::Span<int> { buffer.data() + buffer.size(), 0 }));

    iter = span.iter();
    for (size_t index = 0; index < buffer.size(); ++index)
        ASSERT(*iter++ == buffer[index]);
}

TEST_MAIN();
