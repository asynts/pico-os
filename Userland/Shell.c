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
#include <stdlib.h>
#include <sys/wait.h>
#include <spawn.h>

char* find_executable(const char *name);

int main(int argc, char **argv)
{
    for(;;) {
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
        } else if (strcmp(program, "stat") == 0) {
            const char *path = strtok_r(NULL, " ", &saveptr);
            assert(strtok_r(NULL, " ", &saveptr) == NULL);

            assert(path != NULL);

            int fd = open(path, O_RDONLY);
            assert(fd >= 2);

            struct stat statbuf;
            int retval = fstat(fd, &statbuf);
            assert(retval == 0);

            printf("st_dev: %u\n", statbuf.st_dev);
            printf("st_ino: %u\n", statbuf.st_ino);
            printf("st_mode: %u\n", statbuf.st_mode);
            printf("st_rdev: %u\n", statbuf.st_rdev);
            printf("st_size: %i\n", statbuf.st_size);
            printf("st_blksize: %u\n", statbuf.st_blksize);
            printf("st_blocks: %u\n", statbuf.st_blocks);
            printf("st_uid: %u\n", statbuf.st_uid);
            printf("st_gid: %u\n", statbuf.st_gid);

            retval = close(fd);
            assert(retval == 0);
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

            printf("About to call open\n");

            int fd = open(filename, O_RDONLY);

            char buffer[0x1000];
            for(;;) {
                ssize_t nread = read(fd, buffer, sizeof(buffer));
                assert(nread >= 0);

                ssize_t nwritten = write(STDOUT_FILENO, buffer, nread);
                assert(nwritten == nread);

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
            assert(path != NULL);

            int fd = creat(path, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);
            assert(fd >= 0);

            int retval = close(fd);
            assert(retval == 0);
        } else {
            assert(strtok_r(NULL, " ", &saveptr) == NULL);

            char *fullpath = find_executable(program);

            if (fullpath == NULL) {
                printf("Error: unknown command '%s'\n", program);
            } else {
                int retval;

                char *argv[] = {
                    program,
                    NULL,
                };
                char *envp[] = {
                    NULL,
                };

                pid_t pid;
                retval = posix_spawn(
                    &pid,
                    fullpath,
                    NULL,
                    NULL,
                    argv,
                    envp);
                assert(retval == 0);

                int status;
                retval = wait(&status);
                assert(retval >= 0);

                printf("Child terminated with status %i\n", status);
            }
        }

        free(buffer);
    }
}

char* find_executable(const char *program)
{
    assert(strlen(program) >= 1);

    printf("Searching for program %s\n", program);

    if (program[0] == '/') {
        if (access(program, X_OK) == 0)
            return strdup(program);
        return NULL;
    }

    char *path = strdup(getenv("PATH"));
    char *saveptr;

    for (char *directory = strtok_r(path, ":", &saveptr); directory; directory = strtok_r(NULL, ":", &saveptr)) {
        char *fullpath = malloc(strlen(directory) + strlen(program) + 2);
        fullpath = strcpy(fullpath, directory);
        fullpath = strcat(fullpath, "/");
        fullpath = strcat(fullpath, program);

        printf("Checking %s\n", fullpath);

        if (access(fullpath, X_OK) == 0)
            return fullpath;

        free(fullpath);
    }

    return NULL;
}
