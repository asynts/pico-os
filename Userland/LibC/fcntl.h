#pragma once

#include <sys/stat.h>

int close(int fd);

int creat(const char *path, mode_t mode);
