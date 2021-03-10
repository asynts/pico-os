#include <readline/readline.h>
#include <stdlib.h>
#include <syscalls.h>
#include <stdint.h>
#include <unistd.h>
#include <malloc.h>
#include <assert.h>

uint32_t _syscall(uint32_t syscall, uint32_t arg1, uint32_t arg2, uint32_t arg3);
#define syscall(syscall, arg1, arg2, arg3) _syscall((uint32_t)(syscall), (uint32_t)(arg1), (uint32_t)(arg2), (uint32_t)(arg3))

int sys$readline(int fd, void *buffer, size_t *buffer_size)
{
    return syscall(_SC_readline, fd, buffer, buffer_size);
}

char* readline(const char *prompt)
{
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
