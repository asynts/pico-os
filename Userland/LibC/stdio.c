#include <stdio.h>
#include <stdlib.h>
#include <sys/system.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <errno.h>

static size_t format_integer(int is_negative, unsigned long long value, size_t bits, char *buffer)
{
    size_t buffer_offset = 0;

    if (is_negative)
        buffer[buffer_offset++] = '-';

    buffer[buffer_offset++] = '0';
    buffer[buffer_offset++] = 'x';

    const size_t ndigits = bits / 4;

    buffer_offset += ndigits;
    for (size_t index = 1; index <= ndigits; ++index) {
        buffer[buffer_offset - index] = "0123456789abcdef"[value % 16];
        value /= 16;
    }

    return buffer_offset;
}

int printf(const char *format, ...)
{
    int nwritten = 0;

    va_list ap;
    va_start(ap, format);

    while(format[0]) {
        if (format[0] == '%') {
            format += 1;

            int flag_size = 0;
            int flag_pointer = 0;
            int flag_small = 0;

            if (format[0] == '%') {
                format += 1;

                putchar('%');
                continue;
            }

            if (format[0] == 's') {
                format += 1;

                const char *str = va_arg(ap, const char*);
                nwritten += strlen(str);
                while (*str)
                    putchar(*str++);

                continue;
            }

            while (format[0] == 'h') {
                format += 1;

                ++flag_small;
            }

            if (format[0] == 'p') {
                format += 1;

                flag_pointer = 1;
            }

            if (format[0] == 'z') {
                format += 1;

                flag_size = 1;
            }

            if (format[0] == 'u' || flag_pointer) {
                if (!flag_pointer)
                    format += 1;

                char buffer[256];
                size_t buffer_size = 0;
                if (flag_size || flag_pointer) {
                    size_t value = va_arg(ap, size_t);
                    buffer_size = format_integer(0, value, 8 * sizeof(size_t), buffer);
                } else if (flag_small == 1) {
                    unsigned short value = va_arg(ap, int);
                    buffer_size = format_integer(0, value, 8 * sizeof(unsigned short), buffer);
                } else {
                    assert(flag_small == 0);
                    unsigned value = va_arg(ap, unsigned);
                    buffer_size = format_integer(0, value, 8 * sizeof(unsigned), buffer);
                }

                for (size_t index = 0; index < buffer_size; ++index)
                    putchar(buffer[index]);

                continue;
            }

            if (format[0] == 'i') {
                format += 1;

                char buffer[256];
                size_t buffer_size = 0;

                if (flag_size) {
                    ssize_t value = va_arg(ap, ssize_t);
                    int is_negative = 0;
                    if (value < 0) {
                        value = -value;
                        is_negative = 1;
                    }

                    buffer_size = format_integer(is_negative, (size_t)value, 8 * sizeof(ssize_t), buffer);
                } else {
                    int value = va_arg(ap, int);
                    int is_negative = 0;
                    if (value < 0) {
                        value = -value;
                        is_negative = 1;
                    }

                    buffer_size = format_integer(is_negative, (size_t)value, 8 * sizeof(int), buffer);
                }

                for (size_t index = 0; index < buffer_size; ++index)
                    putchar(buffer[index]);

                continue;
            }

            abort();
        }

        putchar(format[0]);
        ++nwritten;
        ++format;
    }

    va_end(ap);

    return nwritten;
}

int putchar(int ch)
{
    char buffer[1] = { ch };

    int retval = sys$write(STDOUT_FILENO, buffer, sizeof(buffer));
    libc_check_errno(retval);

    return ch;
}

int puts(const char *str)
{
    ssize_t retval;

    retval = sys$write(STDOUT_FILENO, str, strlen(str));
    libc_check_errno(retval);

    retval = putchar('\n');
    libc_check_errno(retval);

    return 0;
}
