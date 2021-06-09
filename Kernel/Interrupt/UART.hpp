#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>
#include <Std/CircularQueue.hpp>

#include <Kernel/Forward.hpp>

namespace Kernel::Interrupt
{
    class UART : public Singleton<UART> {
    public:
        void trigger();

        Optional<usize> read(Bytes);
        Optional<usize> write(ReadonlyBytes);

    private:
        CircularQueue<char, 64> m_input_queue;
        CircularQueue<char, 64> m_output_queue;

        friend Singleton<UART>;
        UART();
    };
}
