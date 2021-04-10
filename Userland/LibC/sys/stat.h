#pragma once

#include <Kernel/Interface/Types.hpp>

int fstat(int fd, struct stat *statbuf);
int stat(const char *pathname, struct stat *statbuf);
