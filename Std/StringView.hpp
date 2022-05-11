#pragma once

#include <Std/Span.hpp>
#include <Std/Optional.hpp>

#include <compare>

namespace Std {

class StringView : public Span<const char> {
public:
    StringView()
    {
    }
    StringView(Span<const char> span)
        : Span<const char>(span)
    {
    }
    StringView(const char *cstring)
        : Span<const char>(cstring, __builtin_strlen(cstring))
    {
    }
    StringView(const char *data, usize size)
        : Span<const char>(data, size)
    {
    }

    Optional<usize> index_of(char ch)
    {
        char *pointer = __builtin_strchr(data(), ch);

        if (pointer == nullptr)
            return {};

        return pointer - data();
    }

    StringView substr(usize index)
    {
        VERIFY(index <= size());
        return { data() + index, size() - index };
    }
    StringView substr(usize start, usize end)
    {
        VERIFY(start <= end);
        VERIFY(end <= size());
        return { data() + start, end - start };
    }

    StringView trim(usize size)
    {
        size = min(this->size(), size);
        return { data(), size };
    }

    void strcpy_to(Span<char> other) const
    {
        VERIFY(other.size() >= size() + 1);

        __builtin_memcpy(other.data(), data(), size());
        other.data()[size()] = 0;
    }

    bool starts_with(char ch) const
    {
        return size() >= 1 && data()[0] == ch;
    }

    std::strong_ordering operator<=>(StringView rhs) const
    {
        if (size() < rhs.size())
            return std::strong_ordering::less;

        if (size() > rhs.size())
            return std::strong_ordering::greater;

        int retval = __builtin_memcmp(data(), rhs.data(), size());

        if (retval < 0) {
            return std::strong_ordering::less;
        } else if (retval > 0) {
            return std::strong_ordering::greater;
        } else {
            return std::strong_ordering::equal;
        }
    }

    bool operator==(StringView rhs) const
    {
        return operator<=>(rhs) == std::strong_ordering::equal;
    }

    ReadonlyBytes bytes() const { return { reinterpret_cast<const u8*>(data()), size() }; }

private:
    void copy_to(Span<char> other) const;
};

}
