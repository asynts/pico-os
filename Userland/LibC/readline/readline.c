#include <readline/readline.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>
#include <sys/system.h>
#include <stdio.h>

char* readline(const char *prompt)
{
    printf("%s", prompt);

    size_t buffer_size = 256;
    char *buffer = malloc(buffer_size);

    size_t index = 0;
    while (index < buffer_size)
    {
        int retval = sys$read(STDIN_FILENO, &buffer[index], 1);
        assert(retval == 1);

        char ch = buffer[index];

        if (ch == 0x7f) {
            if (index == 0)
                continue;

            printf("\e[1D \e[1D");
            buffer[--index] = 0;
            continue;
        }

        ++index;
        putchar(ch);

        if (ch == '\n') {
            assert(index < buffer_size);
            assert(index > 0);
            buffer[index - 1] = 0;

            return buffer;
        }
    }

    abort();
}
