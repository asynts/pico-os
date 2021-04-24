#pragma once

#include <Std/StringBuilder.hpp>
#include <Std/StringView.hpp>
#include <Std/Lexer.hpp>

namespace Std
{
    class Path {
    public:
        Path()
        {
            m_is_absolute = false;
        }
        Path(StringView path)
        {
            VERIFY(path.size() >= 1);

            Lexer lexer { path };

            m_is_absolute = lexer.try_consume('/');

            while (!lexer.eof())
            {
                m_components.append(lexer.consume_until('/'));

                if (!lexer.eof())
                    lexer.try_consume('/');
            }
        }
        Path(const char *path)
            : Path(StringView { path })
        {
        }
        Path(const Path& path)
            : m_is_absolute(path.m_is_absolute)
            , m_components(path.m_components)
        {
        }

        bool is_absolute() const { return m_is_absolute; }
        auto components() const { return m_components.iter(); }

        Path parent() const
        {
            VERIFY(m_components.size() >= 1);

            Path parent;
            parent.m_is_absolute = m_is_absolute;

            for (usize i = 0; i < m_components.size() - 1; ++i)
                parent.m_components.append(m_components[i]);

            return move(parent);
        }

        String filename() const
        {
            VERIFY(m_components.size() >= 1);
            return m_components[m_components.size() - 1];
        }

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
            ASSERT(!rhs.is_absolute());

            Path path = *this;
            path.m_components.extend(rhs.m_components.span());
            return path;
        }

    private:
        bool m_is_absolute;
        Vector<String> m_components;
    };
}
