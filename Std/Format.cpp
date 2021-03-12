#include <Std/Format.hpp>
#include <Std/Concepts.hpp>

#include <hardware/uart.h>

namespace Std {
    void dbgln_raw(StringView str)
    {
        uart_write_blocking(uart0, (const u8*)str.data(), str.size());
        uart_write_blocking(uart0, (const u8*)"\n", 1);
    }

    template<typename T>
    requires Concepts::Integral<T>
    void Formatter<T>::format(StringBuilder& builder, T value)
    {
        if (value < 0) {
            builder.append('-');
            return format(builder, -value);
        }

        builder.append("0x");

        if (value == 0) {
            for (usize index = 0; index < sizeof(value); ++index)
                builder.append("00");
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

        for (usize index3 = 0; index3 < sizeof(value) * 2 - index1; ++index3)
            builder.append('0');

        builder.append(StringView { buffer, index1 });
    }

    template class Formatter<u8>;
    template class Formatter<u16>;
    template class Formatter<u32>;
    template class Formatter<u64>;
    template class Formatter<i8>;
    template class Formatter<i16>;
    template class Formatter<i32>;
    template class Formatter<i64>;
}
