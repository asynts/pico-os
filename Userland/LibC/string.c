#include <string.h>
#include <stdlib.h>

char* strtok_r(char *str, const char *delim, char **saveptr)
{
    abort();
}

int strcmp(const char *lhs, const char *rhs)
{
    abort();
}

void* memset(void *dest, int ch, size_t count)
{
    for (size_t index = 0; index < count; ++index)
        ((char*)dest)[index] = ch;

    return dest;
}
