#include <dirent.h>
#include <stdlib.h>
#include <sys/system.h>
#include <assert.h>
#include <malloc.h>
#include <fcntl.h>

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
    ssize_t retval = read(dirp->fd, &dirp->entry, sizeof(struct dirent));

    if (retval == 0)
        return NULL;

    assert(retval == sizeof(struct dirent));

    return &dirp->entry;
}

int closedir(DIR *dirp)
{
    int retval = close(dirp->fd);
    assert(retval == 0);
    return 0;
}
