#pragma once

#include <Std/StringView.hpp>

namespace Std {
    class String {
    public:
        String()
        {
            m_buffer_size = 1;
            m_buffer = new char[m_buffer_size];
            m_buffer[0] = 0;
        }
        String(StringView view)
        {
            m_buffer_size = view.size() + 1;
            m_buffer = new char[m_buffer_size];
            view.strcpy_to({ m_buffer, m_buffer_size });
        }
        String(const char *str)
            : String(StringView { str })
        {
        }
        String(String&& other)
        {
            *this = move(other);
        }
        ~String()
        {
            delete m_buffer;
        }

        const char* data() const { return m_buffer; }
        usize size() const { return m_buffer_size - 1; }

        Span<const char> span() const { return { data(), size() }; }

        StringView view() const { return { data(), size() }; }

        operator StringView() const { return view(); }

        String& operator=(String&& other)
        {
            m_buffer = exchange(other.m_buffer, nullptr);
            m_buffer_size = exchange(other.m_buffer_size, 0);
            return *this;
        }

        bool operator==(const String& other) const
        {
            return view() == other.view();
        }

    private:
        char *m_buffer;
        usize m_buffer_size;
    };
}
