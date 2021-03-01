#include <dirent.h>
#include <stdlib.h>

DIR* opendir(const char *path)
{
    abort();
}

int closedir(DIR *dirp)
{
    abort();
}

struct dirent* readdir(DIR *dirp)
{
    abort();
}
