#pragma once

typedef int mode_t;

int creat(const char *path, mode_t mode);
int close(int fd);
