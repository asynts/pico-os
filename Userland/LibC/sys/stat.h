#pragma once

#include <Kernel/Interface/stat.h>

int fstat(int fd, struct stat *statbuf);

#define X_OK 1
