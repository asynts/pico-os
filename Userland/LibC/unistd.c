#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/system.h>
#include <sys/types.h>
#include <stdio.h>

int chdir(const char *path)
{
    abort();
}

pid_t fork(void)
{
    pid_t retval = sys$fork();
    printf("fork() returned %zi\n", (ssize_t)retval);
    return retval;
}

int execve(const char *pathname, char **argv, char **envp)
{
    return sys$execve(pathname, argv, envp);
}

// FIXME: Do this properly
int geteuid(void)
{
    return 0;
}
int getegid(void)
{
    return 0;
}

int access(const char *pathname, int mode)
{
    assert(mode == X_OK);

    struct stat statbuf;
    int retval = stat(pathname, &statbuf);
    assert(retval == 0);

    if (statbuf.st_uid == geteuid() && (statbuf.st_mode & S_IXUSR))
        return 0;
    if (statbuf.st_gid == getegid() && (statbuf.st_mode & S_IXGRP))
        return 0;

    // FIXME: Set errno
    return -1;
}
