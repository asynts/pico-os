#pragma once

#include <Std/StringView.hpp>

namespace Std {
    class String {
    public:
        String()
        {
            m_buffer = new char[1];
            m_buffer_size = 1;
            m_buffer[0] = 0;
        }
        String(StringView view)
        {
            m_buffer = new char[view.size() + 1];
            m_buffer_size = view.size() + 1;
            view.copy_to({ m_buffer, m_buffer_size - 1 });
            m_buffer[size()] = 0;
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

    private:
        char *m_buffer;
        usize m_buffer_size;
    };
}
