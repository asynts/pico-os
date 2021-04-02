#include <Std/Forward.hpp>
#include <Std/Format.hpp>

#include <stdlib.h>

#ifdef TEST
# include <stdio.h>
# include <stdarg.h>
#else
# include <pico/printf.h>
#endif

void __crash(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
    vprintf(format, ap);
    va_end(ap);

    abort();
}
