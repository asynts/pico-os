#pragma once

#include <Std/Singleton.hpp>
#include <Std/String.hpp>
#include <Std/CircularQueue.hpp>

namespace Kernel
{
    using namespace Std;

    struct Thread {
        String m_name;
        Optional<u8*> m_stack;
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        void create_thread(StringView name, void (*callback)())
        {
            u8 *stack = new u8[0x400] + 0x400;

            const auto push = [&](u32 value) {
                stack = stack - 4;
                *reinterpret_cast<u32*>(stack) = value;
            };
            const auto align = [&](u32 boundary) {
                if (u32(stack) % boundary != 0)
                    stack -= u32(stack) % boundary;
            };

            // FIXME: Deal with lambdas

            // FIXME: Add wrapper that deals with threads that return

            constexpr u32 thumb_mask = 1 << 24;

            align(8);

            // Unpacked on exception return
            push(thumb_mask); // XPSR
            push(reinterpret_cast<u32>(callback)); // ReturnAddress
            push(0); // LR (R14)
            push(0); // IP (R12)
            push(0); // R3
            push(0); // R2
            push(0); // R1
            push(0); // R0

            // Unpacked by context switch routine
            push(0); // R4
            push(0); // R5
            push(0); // R6
            push(0); // R7
            push(0); // R8
            push(0); // R9
            push(0); // R10
            push(0); // R11

            // FIXME: Mask PendSV for this operation
            m_threads.enqueue({ name, stack });
        }

    private:
        friend Singleton<Scheduler>;
        Scheduler() = default;

        CircularQueue<Thread, 8> m_threads;
    };
}
