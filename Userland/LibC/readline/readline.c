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

    int retval = sys$readline(STDIN_FILENO, buffer, &buffer_size);

    if (retval < 0) {
        if (retval == -ERANGE) {
            buffer = realloc(buffer, buffer_size);
            int retval = sys$readline(STDIN_FILENO, buffer, &buffer_size);
            assert(retval >= 0);

            return buffer;
        }

        free(buffer);
        return NULL;
    }

    return realloc(buffer, buffer_size);
}
