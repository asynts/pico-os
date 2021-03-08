#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <assert.h>
#include <malloc.h>

// START stuff
#include <stdint.h>
#include <stdlib.h>

extern void *__bss_start__;
extern void *__bss_end__;
extern void _fini();
extern void _init();
extern int main(int argc, char **argv);

// The _start symbol provided by newlib makes a few false assumptions about the
// environment it runs in, specifically, we don't have an MMU and thus can't
// provide __bss_start__ and similar symbols.
void _override_start(uint32_t stack_top)
{
    asm volatile("mov sp, %0;"
                 "mov r3, %0;"
                 "blx _stack_init;"
        :
        : "r"(stack_top));

    memset(__bss_start__, 0, __bss_end__ - __bss_start__);

    // FIXME: We crash in here, i think it tries to obtain a lock of some kind.
    atexit(_fini);

    // FIXME: Is it correct that we call _init instead of __libc_init_array?
    _init();

    int retval = main(0, NULL);
    exit(retval);
}
// END stuff

int main(int argc, char **argv) {
    for(;;) {
        char *buffer = readline("\033[1m> \033[0m");
        assert(buffer != NULL);

        char *saveptr = NULL;
        char *program = strtok_r(buffer, " ", &saveptr);
        assert(program != NULL);

        if (strcmp(program, "echo") == 0) {
            const char *prefix = "";

            for (const char *argument; (argument = strtok_r(NULL, " ", &saveptr)); )
                printf("%s%s", prefix, argument);
            printf("\n");
        } else if (strcmp(program, "ls") == 0) {
            const char *path = strtok_r(NULL, " ", &saveptr);
            assert(strtok_r(NULL, " ", &saveptr) == NULL);

            if (path == NULL)
                path = ".";

            DIR *dir = opendir(path);
            assert(dir);

            for (struct dirent *entry; (entry = readdir(dir)); )
                printf("%s\n", entry->d_name);

            int retval = closedir(dir);
            assert(retval == 0);
        } else if (strcmp(program, "cd") == 0) {
            const char *path = strtok_r(NULL, " ", &saveptr);
            assert(strtok_r(NULL, " ", &saveptr) == NULL);
            assert(path != NULL);

            int retval = chdir(path);
            assert(retval == 0);
        } else if (strcmp(program, "touch") == 0) {
            const char *path = strtok_r(NULL, " ", &saveptr);
            assert(strtok_r(NULL, " ", &saveptr) == NULL);

            int fd = creat(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
            assert(fd >= 0);

            int retval = close(fd);
            assert(retval == 0);
        } else {
            printf("\033[1mError: unknown command '%s'\033[0m\n", program);
        }

        free(buffer);
    }
}
