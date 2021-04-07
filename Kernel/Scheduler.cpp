#include <Kernel/Scheduler.hpp>

namespace Kernel
{
    extern "C"
    {
        u8* scheduler_next(u8 *stack);
    }
}
