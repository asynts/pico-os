#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

void assert_failed(const char *condition, const char *filename, size_t line)
{
    printf("assert(%s) failed\n%s:%zu\n", condition, filename, line);
    abort();
}
