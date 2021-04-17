#pragma once

#include <stddef.h>
#include <Kernel/Interface/Types.hpp>

typedef struct {
    int fd;

    // We return a pointer to this in each readdir call
    struct dirent entry;
} DIR;

DIR* opendir(const char *path);
struct dirent* readdir(DIR *dirp);
int closedir(DIR *dirp);
