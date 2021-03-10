#include <fcntl.h>
#include <stdlib.h>

int close(int fd)
{
    abort();
}

int creat(const char *path, mode_t mode)
{
    abort();
}
