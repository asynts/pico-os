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
# include <Kernel/HandlerMode.hpp>
#endif

namespace Std
{
    static void write_output(StringView value)
    {
#if defined(TEST)
        std::cerr << std::string_view { value.data(), value.size() };
#elif defined(KERNEL)
        Kernel::ConsoleFileHandle handle;

        // We do not aquire a mutex here, because the system may not work at
        // this point.
        bool were_interrupts_enabled = Kernel::disable_interrupts();
        handle.write(value.bytes());
        Kernel::restore_interrupts(were_interrupts_enabled);
#endif
    }
    static void write_output(usize value)
    {
#if defined(TEST)
        std::cerr << value;
#elif defined(KERNEL)
        Kernel::ConsoleFileHandle handle;

        StringBuilder builder;
        builder.appendf("{}", value);

        // We do not aquire a mutex here, because the system may not work at
        // this point.
        bool were_interrupts_enabled = Kernel::disable_interrupts();
        handle.write(builder.bytes());
        Kernel::restore_interrupts(were_interrupts_enabled);
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
        write_output("\n");

#if defined(TEST)
        std::abort();
#else
        asm volatile("bkpt #0");
        for(;;)
            asm volatile("wfi");
#endif
    }
}
