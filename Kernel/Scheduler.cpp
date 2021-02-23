#include <pico/time.h>
#include <pico/printf.h>

#include <Kernel/Scheduler.hpp>

namespace Kernel {

static repeating_timer_t scheduler_timer;
static bool scheduler_callback(repeating_timer_t*)
{
    return true;
}

Scheduler::Scheduler()
{
    if (!add_repeating_timer_ms(100, scheduler_callback, nullptr, &scheduler_timer))
        panic("Can not setup scheduler timer.");
}

}
