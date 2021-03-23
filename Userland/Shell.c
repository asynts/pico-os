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
        printf("0x01020304 %zu\n", (size_t)0x01020304);
        printf("0x00000000 %zu\n", (size_t)0);
        printf("0x11223344 %zu\n", (size_t)0x11223344);

        char *buffer = readline("> ");
        assert(buffer != NULL);

        if (strchr(buffer, '\n'))
            *strchr(buffer, '\n') = 0;

        char *saveptr = NULL;
        char *program = strtok_r(buffer, " ", &saveptr);

        if (program == NULL)
            continue;

        if (strcmp(program, "echo") == 0) {
            const char *prefix = "";

            for (const char *argument; (argument = strtok_r(NULL, " ", &saveptr)); ) {
                printf("%s%s", prefix, argument);
                prefix = " ";
            }
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
        } else if (strcmp(program, "cat") == 0) {
            const char *filename = strtok_r(NULL, " ", &saveptr);
            assert(filename != NULL);
            assert(strtok_r(NULL, " ", &saveptr) == NULL);

            int fd = open(filename, O_RDONLY);

            char buffer[0x1000];
            for(;;) {
                printf("Userland: Before read\n");
                ssize_t nread = read(fd, buffer, sizeof(buffer));
                printf("Userland: After read\n");
                assert(nread >= 0);

                printf("Userland: nread=%zu\n", nread);

                printf("Userland: Before write\n");
                ssize_t nwritten = write(STDOUT_FILENO, buffer, nread);
                printf("Userland: After write\n");
                assert(nwritten == nread);

                printf("Userland: nwritten=%zu\n", nwritten);

                if (nread == 0)
                    break;
            }

            close(fd);
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
            printf("Error: unknown command '%s'\n", program);
        }

        free(buffer);
    }
}
