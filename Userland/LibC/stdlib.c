#include <stdlib.h>
#include <assert.h>
#include <string.h>

_Noreturn void abort(void)
{
    for(;;)
        asm volatile("bkpt #0");
}

_Noreturn void exit(int status)
{
    abort();
}

static char __env_PATH[] = "/bin";

char* getenv(const char *name)
{
    assert(strcmp(name, "PATH") == 0);
    return __env_PATH;
}
