#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/system.h>
#include <sys/types.h>
#include <stdio.h>
#include <malloc.h>
#include <errno.h>

int chdir(const char *pathname)
{
    int retval = sys$chdir(pathname);
    libc_check_errno(retval);
    return 0;
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
    libc_check_errno(retval);

    if (statbuf.st_uid == geteuid() && (statbuf.st_mode & S_IXUSR))
        return 0;
    if (statbuf.st_gid == getegid() && (statbuf.st_mode & S_IXGRP))
        return 0;

    errno = EACCES;
    return -1;
}

char* get_current_dir_name(void)
{
    int retval;

    size_t buffer_size = 0;
    retval = sys$get_working_directory(NULL, &buffer_size);
    assert(retval == -ERANGE);

    char *buffer = malloc(buffer_size);
    retval = sys$get_working_directory(buffer, &buffer_size);
    assert(retval == 0);

    return buffer;
}
