#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

int chdir(const char *path)
{
    abort();
}

pid_t fork(void)
{
    abort();
}

int execle(const char *pathname, ...)
{
    abort();
}

int access(const char *pathname, int mode)
{
    abort();
}
