#include <sys/wait.h>
#include <stdlib.h>
#include <sys/system.h>

pid_t wait(int *status)
{
    pid_t retval;

    while ((retval = sys$wait(status)) == -EINTR)
        ;

    return retval;
}
