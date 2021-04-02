#pragma once

#include <Std/StringView.hpp>
#include <Std/Optional.hpp>

namespace Std
{
    class Lexer {
    public:
        explicit Lexer(StringView input)
            : m_input(input)
        {
        }

        bool eof() const
        {
            return m_offset >= m_input.size();
        }

        Optional<char> peek()
        {
            if (eof())
                return {};
            return m_input[m_offset];
        }
        char peek_or_null()
        {
            return peek().value_or(0);
        }

        Optional<char> consume()
        {
            if (!eof())
                return m_input[m_offset++];
            return {};
        }
        Optional<char> consume(char ch)
        {
            ASSERT(ch != 0);

            if (peek_or_null() == ch)
                return consume();
            return {};
        }

        StringView consume_until(char ch)
        {
            usize offset = m_offset;

            while(!eof() && peek_or_null() != ch)
                consume();


            return m_input.substr(offset, m_offset);
        }

    private:
        StringView m_input;
        usize m_offset = 0;
    };
}
