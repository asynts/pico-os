#pragma once

#include <Std/Span.hpp>
#include <Std/Optional.hpp>

namespace Std {

class StringView : public Span<const char> {
public:
    StringView()
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
        index = max(index, size());
        return { data() + index, size() - index };
    }

    StringView trim(usize size)
    {
        size = min(this->size(), size);
        return { data(), size };
    }

    int operator<=>(StringView rhs) const
    {
        return __builtin_strcmp(data(), rhs.data());
    }

    // FIXME: The compiler should be able to deduce this?
    bool operator==(StringView rhs) const
    {
        return (*this <=> rhs) == 0;
    }
};

}
