#pragma once

#include <Std/StringBuilder.hpp>
#include <Std/StringView.hpp>

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

    class Path {
    public:
        Path(StringView path)
        {
            VERIFY(path.size() >= 1);

            Lexer lexer { path };

            m_is_absolute = lexer.consume('/').is_valid();

            while (!lexer.eof())
            {
                m_components.append(lexer.consume_until('/'));

                if (!lexer.eof())
                    lexer.consume('/').must();
            }
        }
        Path(const char *path)
            : Path(StringView { path })
        {
        }

        bool is_absolute() const { return m_is_absolute; }
        auto components() const { return m_components.iter(); }

        String string() const
        {
            StringBuilder builder;

            if (m_is_absolute && components().size() == 0) {
                builder.append('/');
                return builder.string();
            }

            bool put_slash = m_is_absolute;
            for (auto& component : components())
            {
                if (put_slash)
                    builder.append('/');
                else
                    put_slash = true;


                builder.append(component);
            }

            return builder.string();
        }

        Path operator/(const Path& rhs) const
        {
            Path path = *this;
            path.m_components.extend(rhs.m_components.span());
            return path;
        }

    private:
        bool m_is_absolute;
        Vector<String> m_components;
    };

    // FIXME: Get rid of this
    template<typename Callback>
    void iterate_path_components(StringView path, Callback&& callback)
    {
        VERIFY(path.size() >= 1);
        VERIFY(path[0] == '/');
        path = path.substr(1);

        if (path.size() == 0)
            return;

        auto end_index = path.index_of('/');

        if (!end_index.is_valid()) {
            callback(path, true);
            return;
        } else {
            if (callback(path.trim(end_index.value()), false) == IterationDecision::Break)
                return;
            return iterate_path_components(path.substr(end_index.value()), move(callback));
        }
    }

    inline StringBuilder compute_absolute_path(StringView path)
    {
        StringBuilder builder;

        if (path.starts_with('/')) {
            builder.append(path);
            return builder;
        }

        // FIXME: Introduce process seperation
        StringView current_working_directory = "/";

        builder.append(current_working_directory);
        builder.append(path);

        return builder;
    }
}
