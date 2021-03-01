#pragma once

typedef struct {
} DIR;

struct dirent {
    char d_name[256];
};

DIR* opendir(const char *path);
int closedir(DIR *dirp);
struct dirent* readdir(DIR *dirp);
