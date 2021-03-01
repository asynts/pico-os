#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>

int chdir(const char *path)
{
    abort();
}

ssize_t _write (int fd, const void *buf, size_t count)
{
    abort();
}

ssize_t _read (int fd, void *buf, size_t count)
{
    abort();
}

off_t _lseek(int file, off_t offset, int whence)
{
    abort();
}

void* _sbrk(intptr_t increment)
{
    abort();
}

void _exit(int status)
{
    abort();
}

void _getpid(void)
{
    abort();
}

int _isatty(int fd)
{
    abort();
}
