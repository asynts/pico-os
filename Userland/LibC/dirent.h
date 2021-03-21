#pragma once

typedef struct {
    int fd;
} DIR;

struct dirent {
    char d_name[256];
};

DIR* opendir(const char *path);
struct dirent* readdir(DIR *dirp);
int closedir(DIR *dirp);
