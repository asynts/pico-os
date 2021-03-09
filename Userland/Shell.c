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
