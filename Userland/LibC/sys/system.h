#pragma once

#include <stddef.h>
#include <syscalls.h>

int sys$readline(int fd, void *buffer, size_t *size);
int sys$dmesg(const char *message);
size_t sys$write(int fd, const void *buffer, size_t size);
