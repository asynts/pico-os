#include <Std/Format.hpp>
#include <Std/Concepts.hpp>
#include <Std/String.hpp>
#include <Std/Path.hpp>

#if defined(TEST)
# include <iostream>
#elif defined(KERNEL)
# include <Kernel/ConsoleDevice.hpp>
# include <Kernel/KernelMutex.hpp>
# include <Kernel/HandlerMode.hpp>
#else
# error "Only TEST and KERNEL are supported"
#endif

namespace Std
{
    void dbgln_raw(StringView str)
    {
        if (Kernel::is_executing_in_handler_mode())
            return;

#ifdef KERNEL
        // FIXME: For multi-core support, we will need a mutex here.
        //        We would need to mask interrupts and then get the mutex.
        //        However, that will be quite involved, because of the deadlock risk.
        {
            Kernel::MaskedInterruptGuard interrupt_guard;

            StringView prefix = "\e[36m";
            StringView suffix = "\e[0m\n";

            Kernel::ConsoleFileHandle handle;
            handle.write(prefix.bytes());
            handle.write(str.bytes());
            handle.write(suffix.bytes());
        }
#else
        std::cout << "\e[36m" << std::string_view { str.data(), str.size() } << "\e[0m\n";
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
    void Formatter<char>::format(StringBuilder& builder, char value)
    {
        builder.append(value);
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
