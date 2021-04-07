#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

int chdir(const char *path)
{
    abort();
}

pid_t fork(void)
{
    abort();
}

int execle(const char *pathname, ...)
{
    abort();
}

int access(const char *pathname, int mode)
{
    struct stat statbuf;
    int retval = stat(pathname, &statbuf);
    assert(retval >= 0);

    return statbuf.st_mode & X_OK;
}
