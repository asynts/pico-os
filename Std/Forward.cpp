#include <Std/Forward.hpp>
#include <Std/Format.hpp>
#include <Std/Lexer.hpp>

#if !defined(TEST) && !defined(KERNEL)
# error "Only KERNEL and TEST are supported"
#endif

#if defined(TEST)
# include <iostream>
# include <cstdlib>
#elif defined(KERNEL)
# include <Kernel/ConsoleDevice.hpp>
#endif

namespace Std
{
    static void write_output(StringView value)
    {
#if defined(TEST)
        std::cerr << std::string_view { value.data(), value.size() };
#elif defined(KERNEL)
        auto& handle = Kernel::ConsoleFile::the().create_handle();
        handle.write(value.bytes());
#endif
    }
    static void write_output(usize value)
    {
#if defined(TEST)
        std::cerr << value;
#elif defined(KERNEL)
        auto& handle = Kernel::ConsoleFile::the().create_handle();

        StringBuilder builder;
        builder.appendf("{}", value);

        handle.write(builder.bytes());
#endif
    }

    void crash(const char *format, const char *condition, const char *file, usize line)
    {
        Std::Lexer lexer { format };

        while (!lexer.eof()) {
            if (lexer.try_consume("%condition")) {
                write_output(condition);
                continue;
            }

            if (lexer.try_consume("%file")) {
                write_output(file);
                continue;
            }

            if (lexer.try_consume("%line")) {
                write_output(line);
                continue;
            }

            write_output(lexer.consume_until('%'));

            if (lexer.peek_or_null() != '%')
                ASSERT(lexer.eof());
        }

#if defined(TEST)
        std::abort();
#else
        for(;;)
            asm volatile("wfi");
#endif
    }
}
