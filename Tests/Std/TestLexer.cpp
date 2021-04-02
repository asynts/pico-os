#include <Tests/TestSuite.hpp>

#include <Std/Lexer.hpp>

TEST_CASE(lexer)
{
    Std::Lexer lexer { "foo\nbar\nbaz" };

    ASSERT(!lexer.eof());
    ASSERT(lexer.consume().must() == 'f');
    ASSERT(!lexer.consume('x').is_valid());
    ASSERT(lexer.consume_until('\n') == "oo");

    ASSERT(lexer.peek_or_null() == '\n');
    lexer.consume();

    ASSERT(lexer.consume_until('\n') == "bar");
    ASSERT(lexer.consume('\n').is_valid());

    ASSERT(lexer.consume_until('\n') == "baz");
    ASSERT(lexer.eof());
    ASSERT(!lexer.consume('\n').is_valid());
}

TEST_MAIN();
