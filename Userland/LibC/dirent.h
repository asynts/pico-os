#pragma once

typedef struct {
} DIR;

struct dirent {
    const char *d_name;
};

DIR* opendir(const char *name);
struct dirent* readdir(DIR *dirp);
int closedir(DIR *dirp);
