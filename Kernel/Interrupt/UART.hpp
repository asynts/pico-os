#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>
#include <Std/CircularQueue.hpp>

#include <Kernel/Forward.hpp>
#include <Kernel/Result.hpp>

namespace Kernel::Interrupt
{
    class UART : public Singleton<UART> {
    public:
        void trigger();

        KernelResult<usize> read(Bytes);
        KernelResult<usize> write(ReadonlyBytes);

    private:
        CircularQueue<char, 64> m_input_queue;

        friend Singleton<UART>;
        UART();

        static void interrupt();
    };
}
