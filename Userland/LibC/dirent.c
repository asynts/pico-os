#include <dirent.h>
#include <stdlib.h>
#include <sys/system.h>
#include <assert.h>
#include <malloc.h>

DIR* opendir(const char *path)
{
    int fd = sys$open(path, O_DIRECTORY, 0);
    assert(fd >= 0);

    DIR *dirp = malloc(sizeof(DIR));
    dirp->fd = fd;

    return dirp;
}

struct dirent* readdir(DIR *dirp)
{
    abort();
}

int closedir(DIR *dirp)
{
    abort();
}
