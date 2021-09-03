#pragma once

#include <Std/Forward.hpp>

// FIXME: Find a better name for this file

namespace Kernel
{
    inline bool is_executing_in_thread_mode()
    {
        u32 ipsr;
        asm ("mrs %0, ipsr;"
             "isb;"
            : "=r"(ipsr));

        return ipsr == 0;
    }
    inline bool is_executing_in_handler_mode()
    {
        return !is_executing_in_thread_mode();
    }

    inline bool is_using_main_stack_pointer()
    {
        u32 control;
        asm ("mrs %0, control;"
             "isb;"
            : "=r"(control));

        return (control & 2) == 0;
    }

    inline bool is_executing_privileged()
    {
        u32 control;
        asm ("mrs %0, control;"
             "isb;"
            : "=r"(control));

        return (control & 1) == 0;
    }

    // XXX Verify that these functions work.
    inline bool are_interrupts_enabled()
    {
        u32 primask;
        asm volatile("mrs %0, primask;"
            : "=r"(primask));
        return primask;
    }
    inline bool disable_interrupts()
    {
        if (are_interrupts_enabled()) {
            asm volatile("cpsid i;");
            return true;
        }

        return false;
    }
    inline void enable_interrupts()
    {
        asm volatile("cpsie i;");
    }
    inline void restore_interrupts(bool were_enabled)
    {
        if (were_enabled)
            enable_interrupts();
    }

    // FIXME: Some of this code is redundant with the scheduler

    template<typename T, void (T::*Method)()>
    void call_member_function(T& object)
    {
        (object.*Method)();
    }
}
