#pragma once

#include <stddef.h>
#include <syscalls.h>

int sys$readline(int fd, void *buffer, size_t *buffer_size);
int sys$dmesg(const char *message);
