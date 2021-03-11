#pragma once

#include <stdarg.h>
#include <pico/printf.h>
#include <hardware/uart.h>

inline void dbgprintf(const char *format, ...)
{
    char buffer[256];

    va_list ap;
    va_start(ap, format);
    vsnprintf(buffer, sizeof(buffer), format, ap);
    va_end(ap);

    uart_write_blocking(uart0, (uint8_t*)buffer, __builtin_strlen(buffer));
}

