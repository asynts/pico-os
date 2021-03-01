#include <stdlib.h>

void abort(void)
{
    asm volatile("1:"
                 "wfi;"
                 "b 1b;");
}
