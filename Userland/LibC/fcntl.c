#include <fcntl.h>
#include <stdlib.h>
#include <sys/system.h>
#include <stdarg.h>
#include <errno.h>

int close(int fd)
{
    int retval = sys$close(fd);
    libc_check_errno(retval);
    return 0;
}

int open(const char *path, int flags, ...)
{
    if ((flags & O_CREAT))
    {
        va_list ap;

        va_start(ap, flags);
        int mode = va_arg(ap, int);
        va_end(ap);

        int retval = sys$open(path, flags, mode);
        libc_check_errno(retval);
        return retval;
    }

    int retval = sys$open(path, flags, 0);
    libc_check_errno(retval);
    return retval;
}

ssize_t read(int fd, void *buffer, size_t count)
{
    ssize_t retval = sys$read(fd, buffer, count);
    libc_check_errno(retval);
    return retval;
}

ssize_t write(int fd, const void *buffer, size_t count)
{
    ssize_t retval = sys$write(fd, buffer, count);
    libc_check_errno(retval);
    return retval;
}
