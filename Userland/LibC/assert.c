#include <assert.h>

void breakpoint(const char *filename, size_t line)
{
    asm volatile("bkpt #0");
}
