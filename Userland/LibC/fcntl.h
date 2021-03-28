#pragma once

#include <sys/stat.h>
#include <sys/types.h>
#include <Kernel/Interface/syscalls.h>

int close(int fd);

int creat(const char *path, mode_t mode);

int open(const char *path, int flags, ...);

ssize_t read(int fd, void *buffer, size_t count);
ssize_t write(int fd, const void *buffer, size_t count);
