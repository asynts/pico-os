#include <dirent.h>
#include <stdlib.h>
#include <sys/system.h>
#include <assert.h>
#include <malloc.h>
#include <fcntl.h>
#include <errno.h>

DIR* opendir(const char *path)
{
    int fd = sys$open(path, O_DIRECTORY, 0);

    if (fd < 0) {
        errno = -fd;
        return NULL;
    }

    DIR *dirp = malloc(sizeof(DIR));
    dirp->fd = fd;

    return dirp;
}

struct dirent* readdir(DIR *dirp)
{
    ssize_t retval = read(dirp->fd, &dirp->entry, sizeof(struct dirent));

    if (retval < 0) {
        errno = -retval;
        return NULL;
    }

    if (retval == 0)
        return NULL;

    assert(retval == sizeof(struct dirent));

    return &dirp->entry;
}

int closedir(DIR *dirp)
{
    int retval = close(dirp->fd);
    libc_check_errno(retval);
    return 0;
}
