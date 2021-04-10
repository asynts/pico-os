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

        char consume()
        {
            ASSERT(!eof());
            return m_input[m_offset++];
        }
        bool try_consume(char ch)
        {
            ASSERT(ch != 0);

            if (peek_or_null() == ch) {
                consume();
                return true;
            }
            return {};
        }
        bool try_consume(StringView str)
        {
            if (str.size() > remaining())
                return false;

            for (usize i = 0; i < str.size(); ++i) {
                if (m_input[m_offset + i] != str[i])
                    return false;
            }

            m_offset += str.size();
            return true;
        }

        StringView consume_until(char ch)
        {
            usize offset = m_offset;

            while(!eof() && peek_or_null() != ch)
                consume();


            return m_input.substr(offset, m_offset);
        }

        usize remaining() const { return m_input.size() - m_offset; }

    private:
        StringView m_input;
        usize m_offset = 0;
    };
}
