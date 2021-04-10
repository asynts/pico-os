#pragma once

#include <stddef.h>
#include <Kernel/Interface/System.hpp>
#include <sys/stat.h>

int sys$dmesg(const char *message);
ssize_t sys$write(int fd, const void *buffer, size_t count);
ssize_t sys$read(int fd, void *buffer, size_t count);
int sys$open(const char *path, int flags, int mode);
int sys$close(int fd);
int sys$fstat(int fd, struct stat *statbuf);
int sys$stat(const char *pathname, struct stat *statbuf);
