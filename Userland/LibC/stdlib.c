#include <stdlib.h>

_Noreturn void abort(void)
{
    for(;;)
        asm volatile("bkpt #0");
}

_Noreturn void exit(int status)
{
    abort();
}
