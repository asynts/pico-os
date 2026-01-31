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

static char __libc_environment_PATH[] = "/bin";
#define environment_PATH access_mutable_global_array(__libc_environment_PATH, char)

char* getenv(const char *name)
{
    assert(strcmp(name, "PATH") == 0);
    return environment_PATH;
}
