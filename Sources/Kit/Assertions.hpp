#pragma once

#include <Kit/Forward.hpp>

namespace Kit
{
    inline void BREAKPOINT() {
#ifdef __arm__
        asm volatile("bkpt #0");
#else
        asm volatile("int3");
#endif
    }

    [[noreturn]]
    inline void ASSERT_NOT_REACHED() {
        for (;;) {
            BREAKPOINT();
        }
    }

    inline void ASSERT(bool condition) {
        if (!condition) {
            for(;;) {
                BREAKPOINT();
            }
        }
    }

    inline void FIXME() {
        BREAKPOINT();
    }
}
