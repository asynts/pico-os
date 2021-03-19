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
        ~String()
        {
            delete m_buffer;
        }

        const char* data() const { return m_buffer; }
        usize size() const { return m_buffer_size - 1; }

        Span<const char> span() const { return { data(), size() }; }

        StringView view() const { return { data(), size() }; }

        bool operator==(const String& other) const
        {
            return view() == other.view();
        }

    private:
        char *m_buffer;
        usize m_buffer_size;
    };
}
