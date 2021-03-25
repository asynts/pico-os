#include <Std/Forward.hpp>
#include <Std/Format.hpp>

#include <stdlib.h>
#include <pico/printf.h>

void __crash(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);

    abort();
}
