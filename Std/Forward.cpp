#include <Std/Forward.hpp>
#include <Std/Format.hpp>

#include <stdlib.h>

void __crash(const char *prefix, const char *filename, usize line)
{
    dbgln("% %:%", prefix, filename, line);
    abort();
}
