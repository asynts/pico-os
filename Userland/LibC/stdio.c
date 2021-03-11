#include <stdio.h>
#include <stdlib.h>
#include <sys/system.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

int printf(const char *format, ...)
{
    int nwritten = 0;

    va_list ap;
    va_start(ap, format);

    while(format[0]) {
        if (format[0] == '%') {
            if (format[1] == '%') {
                putchar('%');
                format += 2;
                continue;
            }

            if (format[1] == 's') {
                const char *str = va_arg(ap, const char*);
                nwritten += strlen(str);
                while (*str)
                    putchar(*str++);

                format += 2;
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
    sys$write(STDOUT_FILENO, buffer, sizeof(buffer));

    return ch;
}

int puts(const char *str)
{
    sys$write(STDOUT_FILENO, str, strlen(str));
    putchar('\n');
    return 0;
}
