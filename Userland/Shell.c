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
#include <errno.h>

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

            if (strtok_r(NULL, " ", &saveptr) != NULL) {
                printf("stat: Trailing arguments\n");
                goto next_iteration;
            }

            if (path == NULL) {
                printf("stat: Missing operand\n");
                goto next_iteration;
            }

            struct stat statbuf;
            int retval = stat(path, &statbuf);

            if (retval < 0) {
                printf("stat: %s\n", strerror(errno));
                goto next_iteration;
            }

            printf("st_dev: %u\n", statbuf.st_dev);
            printf("st_ino: %u\n", statbuf.st_ino);
            printf("st_mode: %u\n", statbuf.st_mode);
            printf("st_rdev: %u\n", statbuf.st_rdev);
            printf("st_size: %i\n", statbuf.st_size);
            printf("st_blksize: %u\n", statbuf.st_blksize);
            printf("st_blocks: %u\n", statbuf.st_blocks);
            printf("st_uid: %u\n", statbuf.st_uid);
            printf("st_gid: %u\n", statbuf.st_gid);
        } else if (strcmp(program, "ls") == 0) {
            const char *path = strtok_r(NULL, " ", &saveptr);

            if (strtok_r(NULL, " ", &saveptr) != NULL) {
                printf("ls: Trailing arguments\n");
                goto next_iteration;
            }

            if (path == NULL)
                path = ".";

            DIR *dir = opendir(path);

            if (dir == NULL) {
                printf("ls: %s\n", strerror(errno));
                goto next_iteration;
            }

            for(;;) {
                errno = 0;
                struct dirent *entry = readdir(dir);

                if (entry == NULL) {
                    if (errno < 0) {
                        printf("ls: %s\n", strerror(errno));
                        goto next_iteration;
                    }

                    break;
                }

                printf("%s\n", entry->d_name);
            }

            closedir(dir);
        } else if (strcmp(program, "cat") == 0) {
            const char *filename = strtok_r(NULL, " ", &saveptr);

            if (filename == NULL) {
                printf("stat: Missing operand\n");
                goto next_iteration;
            }

            if (strtok_r(NULL, " ", &saveptr) != NULL) {
                printf("cat: Trailing arguments\n");
                goto next_iteration;
            }

            int fd = open(filename, O_RDONLY);

            if (fd < 0) {
                printf("cat: %s\n", strerror(errno));
                goto next_iteration;
            }

            char buffer[0x1000];
            for(;;) {
                ssize_t nread = read(fd, buffer, sizeof(buffer));

                if (nread < 0) {
                    printf("cat: %s\n", strerror(errno));
                    goto next_iteration;
                }

                ssize_t nwritten = write(STDOUT_FILENO, buffer, nread);
                assert(nwritten == nread);

                if (nread == 0)
                    break;
            }

            close(fd);
        } else if (strcmp(program, "cd") == 0) {
            const char *path = strtok_r(NULL, " ", &saveptr);

            if (path == NULL) {
                printf("cd: Missing operand\n");
                goto next_iteration;
            }

            if (strtok_r(NULL, " ", &saveptr) != NULL) {
                printf("cd: Trailing arguments\n");
                goto next_iteration;
            }

            int retval = chdir(path);

            if (retval < 0) {
                printf("cd: %s\n", strerror(errno));
                goto next_iteration;
            }
        } else if (strcmp(program, "touch") == 0) {
            const char *path = strtok_r(NULL, " ", &saveptr);

            if (path == NULL) {
                printf("touch: Missing operand\n");
                goto next_iteration;
            }

            if (strtok_r(NULL, " ", &saveptr) != NULL) {
                printf("touch: Trailing arguments\n");
                goto next_iteration;
            }

            int fd = open(path, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

            if (fd < 0) {
                printf("touch: %s\n", strerror(errno));
                goto next_iteration;
            }

            close(fd);
        } else {
            if (strlen(program) < 1) {
                printf("sh: %s\n", strerror(ENOENT));
                goto next_iteration;
            }

            char *saveptr_clone = strdup(saveptr);
            int argc = 1;
            while (strtok_r(NULL, " ", &saveptr_clone) != NULL)
                ++argc;

            char **argv = malloc((argc + 1) * sizeof(char*));
            argv[0] = program;
            argv[argc] = NULL;

            for (size_t i = 1; i < argc; ++i)
                argv[i] = strtok_r(NULL, " ", &saveptr);

            char *fullpath = find_executable(program);

            if (fullpath == NULL) {
                printf("sh: %s\n", strerror(ENOENT));
                goto next_iteration;
            } else {
                int retval;

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

                if (retval < 0) {
                    printf("sh: %s\n", strerror(errno));
                    goto next_iteration;
                }

                int status;
                retval = wait(&status);

                if (retval < 0) {
                    printf("sh: %s\n", strerror(errno));
                    goto next_iteration;
                }

                printf("Child terminated with status %i\n", status);
            }
        }

    next_iteration:
        free(buffer);
    }
}

char* find_executable(const char *program)
{
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

        if (access(fullpath, X_OK) == 0)
            return fullpath;

        free(fullpath);
    }

    return NULL;
}
