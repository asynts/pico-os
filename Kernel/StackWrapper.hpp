#pragma once

#include <Std/Span.hpp>

#include <Kernel/Forward.hpp>

namespace Kernel
{
    struct StackWrapper {
        explicit StackWrapper(Bytes bytes)
            : m_bytes(bytes)
            , m_top(bytes.data() + bytes.size())
        {
        }

        u8* reserve(usize count)
        {
            ASSERT(m_bytes.data() + count <= m_top);
            return m_top -= count;
        }

        u8* push(ReadonlyBytes bytes)
        {
            u8 *data = reserve(bytes.size());
            bytes.copy_to({ data, bytes.size() });
            return data;
        }

        template<typename T>
        T* push_value(const T& value)
        {
            u8 *data = reserve(sizeof(value));
            return new (data) T { value };
        }

        char* push_cstring(const char *cstring)
        {
            reserve(__builtin_strlen(cstring) + 1);

            // FIXME: Is this necessary?
            char *cstring_on_stack = (char*)align(4);

            __builtin_strcpy(cstring_on_stack, cstring);
            return cstring_on_stack;
        }

        u8* align(u32 boundary)
        {
            static_assert(sizeof(u8*) == sizeof(u32));
            if (u32(m_top) % boundary != 0)
                reserve(u32(m_top) % boundary);

            return m_top;
        }

        u8* top()
        {
            return m_top;
        }

    private:
        Bytes m_bytes;
        u8 *m_top;
    };
}
