#pragma once

#include <Kernel/Interface/stat.h>

int fstat(int fd, struct stat *statbuf);
int stat(const char *pathname, struct stat *statbuf);
