#include <sys/stat.h>
#include <sys/system.h>
#include <stdlib.h>
#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>

int fstat(int fd, struct stat *statbuf)
{
    int retval = sys$fstat(fd, statbuf);
    libc_check_errno(retval);
    return 0;
}

int stat(const char *pathname, struct stat *statbuf)
{
    int fd = sys$open(pathname, O_RDONLY, 0);

    if (fd < 0) {
        errno = ENOENT;
        return -1;
    }

    int retval = sys$fstat(fd, statbuf);

    sys$close(fd);

    libc_check_errno(retval);
    return 0;
}
