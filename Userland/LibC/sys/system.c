#include <sys/system.h>
#include <stdint.h>

uint32_t _syscall(uint32_t syscall, uint32_t arg1, uint32_t arg2, uint32_t arg3);
#define syscall(syscall, arg1, arg2, arg3) _syscall((uint32_t)(syscall), (uint32_t)(arg1), (uint32_t)(arg2), (uint32_t)(arg3))

inline int sys$readline(int fd, void *buffer, size_t *size)
{
    return syscall(_SC_readline, fd, buffer, size);
}

inline int sys$dmesg(const char *message)
{
    return syscall(_SC_dmesg, message, 0, 0);
}

size_t sys$write(int fd, const void *buffer, size_t size)
{
    return syscall(_SC_write, fd, buffer, size);
}
