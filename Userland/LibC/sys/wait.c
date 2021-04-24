#include <sys/wait.h>
#include <stdlib.h>
#include <sys/system.h>
#include <errno.h>

pid_t wait(int *status)
{
    pid_t retval;

    while ((retval = sys$wait(status)) == -EINTR)
        ;

    libc_check_errno(retval);
    return 0;
}
