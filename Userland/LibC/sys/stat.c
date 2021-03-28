#include <sys/stat.h>
#include <sys/system.h>

int fstat(int fd, struct stat *statbuf)
{
    return sys$fstat(fd, statbuf);
}
