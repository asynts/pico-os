#include <fcntl.h>
#include <stdlib.h>
#include <sys/system.h>
#include <stdarg.h>

int close(int fd)
{
    return sys$close(fd);
}

int creat(const char *path, mode_t mode)
{
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

int open(const char *path, int flags, ...)
{
    if ((flags & O_CREAT))
    {
        va_list ap;

        va_start(ap, flags);
        int mode = va_arg(ap, int);
        va_end(ap);

        return sys$open(path, flags, mode);
    }

    return sys$open(path, flags, 0);
}

ssize_t read(int fd, void *buffer, size_t count)
{
    return sys$read(fd, buffer, count);
}

ssize_t write(int fd, const void *buffer, size_t count)
{
    return sys$write(fd, buffer, count);
}
