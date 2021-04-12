#include <sys/wait.h>
#include <stdlib.h>
#include <sys/system.h>

pid_t wait(int *status)
{
    while (sys$wait(status) == -EINTR)
        ;

    abort();
}
