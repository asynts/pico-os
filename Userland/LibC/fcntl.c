#include <fcntl.h>
#include <stdlib.h>
#include <sys/system.h>

int close(int fd)
{
    abort();
}

int creat(const char *path, mode_t mode)
{
    abort();
}

int open(const char *path, int flags, ...)
{
    // FIXME: Deal with O_CREAT
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
