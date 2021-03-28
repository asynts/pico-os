#pragma once

#include <stddef.h>

struct dirent {
    char d_name[256];
};

typedef struct {
    int fd;
    struct dirent *entries;
    size_t nentries;
} DIR;

DIR* opendir(const char *path);
struct dirent* readdir(DIR *dirp);
int closedir(DIR *dirp);
