#include <stdlib.h>
#include <sys/stat.h>

int _fstat(int file, struct stat *statbuf)
{
    abort();
}
