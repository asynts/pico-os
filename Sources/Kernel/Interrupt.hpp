#pragma once

#include <Kit/Forward.hpp>

namespace Kernel
{
    // -   It could happen, that this function is called in a nested context.
    //     In that case we need to be careful that we don't re-enable interrupts too early.
    //
    // -   We do not need to syncronize with the other core, because interrupts are enabled or
    //     disabled for each core seperately.
    //
    // -   We do not need to synchronize with other threads, because when we mask interrupts, the scheduler
    //     interrupt is masked as well.
    template<typename T>
    void with_interrupts_disabled(T&& callback)
    {
        u32 primask;

        // FIXME: Test that this actually works.

        // This isn't actually atomic, but this doesn't matter.
        // If an exception occurs between the 'mrs' and 'cpsid' instructions we assume that the exception handler restores the
        // state of PRIMASK.
        asm volatile ("mrs %[primask], primask;"
                    "cpsid i;"
            : [primask] "=r"(primask));

        callback();

        asm volatile ("msr primask, %[primask];"
                    "isb;"
            :
            : [primask] "r"(primask));
    }
}
