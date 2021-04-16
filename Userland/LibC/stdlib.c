#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <sys/system.h>

_Noreturn void abort(void)
{
    for(;;)
        asm volatile("bkpt #0");
}

_Noreturn void exit(int status)
{
    sys$exit(status);
    printf("How did we get here?\n");
}

static char __env_PATH[] = "/bin";

char* getenv(const char *name)
{
    assert(strcmp(name, "PATH") == 0);
    return __env_PATH;
}
