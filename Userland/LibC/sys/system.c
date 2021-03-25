#include <sys/system.h>
#include <stdint.h>
#include <stdio.h>

int32_t _syscall(uint32_t syscall, uint32_t arg1, uint32_t arg2, uint32_t arg3);
#define syscall(syscall, arg1, arg2, arg3) _syscall((uint32_t)(syscall), (uint32_t)(arg1), (uint32_t)(arg2), (uint32_t)(arg3))

ssize_t sys$write(int fd, const void *buffer, size_t count)
{
    return syscall(_SC_write, fd, buffer, count);
}

ssize_t sys$read(int fd, void *buffer, size_t count)
{
    ssize_t nread = syscall(_SC_read, fd, buffer, count);

    if (fd >= 2)
        printf("[sys$read] nread=%zi\n", nread);

    return nread;
}

int sys$open(const char *path, int flags, int mode)
{
    return syscall(_SC_open, path, flags, mode);
}

int sys$close(int fd)
{
    return syscall(_SC_close, fd, 0, 0);
}
