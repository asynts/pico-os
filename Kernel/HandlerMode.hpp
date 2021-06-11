#pragma once

#include <Std/Forward.hpp>

namespace Kernel
{
    inline bool is_executing_in_thread_mode() {
        u32 ipsr;
        asm ("mrs %0, ipsr;"
             "isb;"
            : "=r"(ipsr));

        return ipsr == 0;
    }

    // FIXME: Some of this code is redundant with the scheduler

    template<typename T, void (T::*Method)()>
    void call_member_function(T& object)
    {
        (object.*Method)();
    }
}
