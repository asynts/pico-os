#include <sys/stat.h>
#include <sys/system.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>

int fstat(int fd, struct stat *statbuf)
{
    return sys$fstat(fd, statbuf);
}

int stat(const char *pathname, struct stat *statbuf)
{
    // FIXME: This is really ugly
    int fd = sys$open(pathname, O_RDONLY, 0);

    int retval = sys$fstat(fd, statbuf);

    sys$close(fd);

    return retval;
}
