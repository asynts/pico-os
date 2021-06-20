#include <sys/system.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int32_t _syscall(uint32_t syscall, uint32_t arg1, uint32_t arg2, uint32_t arg3);
#define syscall(syscall, arg1, arg2, arg3) _syscall((uint32_t)(syscall), (uint32_t)(arg1), (uint32_t)(arg2), (uint32_t)(arg3))

ssize_t sys$write(int fd, const void *buffer, size_t count)
{
    return syscall(_SC_write, fd, buffer, count);
}

ssize_t sys$read(int fd, void *buffer, size_t count)
{
    return syscall(_SC_read, fd, buffer, count);
}

int sys$open(const char *pathname, int flags, int mode)
{
    return syscall(_SC_open, pathname, flags, mode);
}

int sys$close(int fd)
{
    return syscall(_SC_close, fd, 0, 0);
}

int sys$fstat(int fd, struct stat *statbuf)
{
    return syscall(_SC_fstat, fd, statbuf, 0);
}

int sys$wait(int *wstatus)
{
    return syscall(_SC_wait, wstatus, 0, 0);
}

void sys$exit(int status)
{
    syscall(_SC_exit, status, 0, 0);
    asm volatile("bkpt #0");
    printf("sys$exit returned?\n");
    abort();
}

int sys$chdir(const char *pathname)
{
    return syscall(_SC_chdir, pathname, 0, 0);
}

int sys$posix_spawn(
    pid_t *pid,
    const char *pathname,
    const posix_spawn_file_actions_t *file_actions,
    const posix_spawnattr_t *attrp,
    char **argv,
    char **envp)
{
    struct extended_system_call_arguments extended_arguments;
    extended_arguments.arg3 = (uint32_t)file_actions;
    extended_arguments.arg4 = (uint32_t)attrp;
    extended_arguments.arg5 = (uint32_t)argv;
    extended_arguments.arg6 = (uint32_t)envp;

    return syscall(_SC_posix_spawn, pid, pathname, &extended_arguments);
}

int sys$get_working_directory(void *buffer, size_t *buffer_size)
{
    return syscall(_SC_get_working_directory, buffer, buffer_size, 0);
}
