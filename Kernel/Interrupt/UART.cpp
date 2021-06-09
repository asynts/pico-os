#include <Kernel/Interrupt/UART.hpp>
#include <Kernel/Interrupt/InterruptGuard.hpp>

namespace Kernel::Interrupt
{
    UART::UART()
    {
        // FIXME: Setup interrupts
    }

    Optional<usize> UART::read(Bytes bytes)
    {
        InterruptGuard interrupt_guard;

        usize index;
        for (index = 0; index < min(bytes.size(), m_input_queue.size()); ++index) {
            bytes[index] = m_input_queue.dequeue();
        }

        return index;
    }

    Optional<usize> UART::write(ReadonlyBytes bytes)
    {
        InterruptGuard interrupt_guard;

        usize index;
        for (index = 0; index < min(bytes.size(), m_output_queue.avaliable()); ++index) {
            m_output_queue.enqueue(bytes[index]);
        }

        return index;
    }
}
