#include <dirent.h>
#include <stdlib.h>

DIR* opendir(const char *path)
{
    abort();
}

struct dirent* readdir(DIR *dirp)
{
    abort();
}

int closedir(DIR *dirp)
{
    abort();
}
