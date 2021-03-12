#include <Std/Format.hpp>
#include <Std/Concepts.hpp>

namespace Std {
    template<typename T>
    requires Concepts::Integral<T>
    void format_arg(StringBuilder& builder, T value)
    {
        if (value < 0) {
            builder.append('-');
            return format_arg(builder, -value);
        }

        builder.append("0x");

        if (value == 0) {
            builder.append('0');
            return;
        }

        static_assert(sizeof(T) <= 8);
        char buffer[16];

        usize index1 = 0;
        while (value > 0) {
            buffer[index1++] = "0123456789abcdef"[value % 16];
            value /= 16;
        }

        for (usize index2 = 0; index2 < index1 / 2; ++index2)
            buffer[index2] = buffer[(index1 - 1) - index2];

        builder.append(StringView { buffer, index1 });
    }

    template void format_arg<u8>(StringBuilder&, u8);
    template void format_arg<u16>(StringBuilder&, u16);
    template void format_arg<u32>(StringBuilder&, u32);
    template void format_arg<u64>(StringBuilder&, u64);
    template void format_arg<i8>(StringBuilder&, i8);
    template void format_arg<i16>(StringBuilder&, i16);
    template void format_arg<i32>(StringBuilder&, i32);
    template void format_arg<i64>(StringBuilder&, i64);
}
