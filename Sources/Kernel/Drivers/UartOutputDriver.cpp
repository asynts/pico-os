#include <Kernel/Drivers/UartOutputDriver.hpp>

#include <hardware/structs/dma.h>

// CLAIM: UART0
// CLAIM: DMA_CHANNEL0

// FIXME: Add some interrupt guard helper.

namespace Kernel::Drivers
{
    UartOutputDriver::UartOutputDriver() {

    }

    // If the DMA is currently doing something, this will return a stale value.
    // This is fine, but the caller needs to be aware of this.
    usize UartOutputDriver::consumer_offset_snapshot()
    {
        for (usize i = 0; i < 3; ++i) {
            usize interrupt_counter = m_interrupt_counter;

            usize result = m_consumer_offset_base + (dma_hw->ch[0].read_addr - reinterpret_cast<usize>(m_buffer));

            if (interrupt_counter == m_interrupt_counter)
                return result;
        }

        // In my mind, it should be sufficent to retry twice.
        // Since only one thread can interact with the 'UartOutputDriver', the worst case should be:
        //
        //  1. We start executing 'consumer_offset_snapshot'.
        //
        //  2. The UART is done and raises an interrupt.
        //
        //  3. The interrupt handler updates the transaction count.
        //
        //  4. The UART is done again.
        //
        //  5. The interrupt handler leaves the transaction count at zero.
        //
        // In this scenario, we can only fail twice.
        ASSERT_NOT_REACHED();
    }

    void UartOutputDriver::write(ReadonlyBytes bytes) {
        // FIXME
        (void)bytes;
    }

    void UartOutputDriver::interrupt_end_of_transmission() {
        // FIXME
    }
}
