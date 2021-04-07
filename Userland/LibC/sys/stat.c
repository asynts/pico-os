#include <sys/stat.h>
#include <sys/system.h>
#include <stdlib.h>
#include <assert.h>

int fstat(int fd, struct stat *statbuf)
{
    return sys$fstat(fd, statbuf);
}

int stat(const char *pathname, struct stat *statbuf)
{
    int retval = sys$stat(pathname, statbuf);
    assert(retval == 0);

    return 0;
}
