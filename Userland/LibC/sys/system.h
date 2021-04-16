#pragma once

#include <stddef.h>
#include <Kernel/Interface/System.hpp>
#include <Kernel/Interface/Types.hpp>
#include <sys/stat.h>
#include <sys/types.h>

ssize_t sys$write(int fd, const void *buffer, size_t count);
ssize_t sys$read(int fd, void *buffer, size_t count);
int sys$open(const char *pathname, int flags, int mode);
int sys$close(int fd);
int sys$fstat(int fd, struct stat *statbuf);
pid_t sys$fork(void);
int sys$wait(int *wstatus);
int sys$execve(const char *pathname, char **argv, char **envp);

_Noreturn
void sys$exit(int status);
