#include <Std/Format.hpp>
#include <Std/Concepts.hpp>
#include <Std/String.hpp>
#include <Std/Path.hpp>

#ifdef TEST
# include <iostream>
# include <stdarg.h>
#else
# include <hardware/uart.h>
#endif

namespace Std {
    void dbgln_raw(StringView str)
    {
#ifdef TEST
        std::cout << "\e[36m" << std::string_view { str.data(), str.size() } << "\e[0m\n";
#else
        uart_write_blocking(uart0, (const u8*)"\e[36m", 5);
        uart_write_blocking(uart0, (const u8*)str.data(), str.size());
        uart_write_blocking(uart0, (const u8*)"\e[0m\n", 5);
#endif
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

        char buffer[sizeof(T) * 2];
        for (usize index = 0; index < sizeof(buffer); ++index) {
            buffer[index] = "0123456789abcdef"[value % 16];
            value /= 16;
        }

        for (usize index = 0; index < sizeof(buffer); ++index)
            builder.append(buffer[(sizeof(buffer) - 1) - index]);
    }

    void Formatter<StringView>::format(StringBuilder& builder, StringView value)
    {
        builder.append(value);
        return;
    }

    void Formatter<bool>::format(StringBuilder& builder, bool value)
    {
        if (value)
            builder.append("true");
        else
            builder.append("false");
    }

    void Formatter<Path>::format(StringBuilder& builder, const Path& value)
    {
        builder.append(value.string());
    }


    template class Formatter<unsigned char>;
    template class Formatter<unsigned short>;
    template class Formatter<unsigned int>;
    template class Formatter<unsigned long>;
    template class Formatter<unsigned long long>;
    template class Formatter<signed char>;
    template class Formatter<signed short>;
    template class Formatter<signed int>;
    template class Formatter<signed long>;
    template class Formatter<signed long long>;
}
