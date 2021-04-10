#include <Tests/TestSuite.hpp>

#include <Std/Lexer.hpp>

TEST_CASE(lexer)
{
    Std::Lexer lexer { "foo\nbar\nbaz" };

    ASSERT(!lexer.eof());
    ASSERT(lexer.consume() == 'f');
    ASSERT(!lexer.try_consume('x'));
    ASSERT(lexer.consume_until('\n') == "oo");

    ASSERT(lexer.peek_or_null() == '\n');
    lexer.consume();

    ASSERT(lexer.consume_until('\n') == "bar");
    ASSERT(lexer.try_consume('\n'));

    ASSERT(lexer.consume_until('\n') == "baz");
    ASSERT(lexer.eof());
    ASSERT(!lexer.try_consume('\n'));
}

TEST_MAIN();
