#include <Kernel/Drivers/UartOutputDriver.hpp>

#include <hardware/structs/dma.h>

// CLAIM: UART0
// CLAIM: DMA_CHANNEL0

// FIXME: Add some interrupt guard helper.
// FIXME: Consider using 'temporarily_disable_interrupts' more.
// FIXME: Consistency: transfer, transmission, etc.

// FIXME: Move this somewhere else.
bool disable_interrupts() {
    // FIXME
    return true;
}
void restore_interrupts(bool enable) {
    // FIXME
    (void)enable;
}
template<typename T>
void temporarily_disable_interrupts(T&& callback)
{
    bool were_enabled = disable_interrupts();
    callback();
    restore_interrupts(were_enabled);
}

namespace Kernel::Drivers
{
    UartOutputDriver::UartOutputDriver() {

    }

    // If the DMA is currently doing something, this will return a stale value.
    // This is fine, but the caller needs to be aware of this.
    usize UartOutputDriver::consumer_offset_snapshot() {
        for (usize retry = 0; retry <= 2; ++retry) {
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

    // If the DMA is currently doing something, this will return a stale value.
    // This is fine, but the caller needs to be aware of this.
    usize UartOutputDriver::avaliable_space_snapshot() {
        return sizeof(m_buffer) - (m_producer_offset - consumer_offset_snapshot());
    }

    usize UartOutputDriver::try_write(ReadonlyBytes bytes) {
        usize nwritten = min(avaliable_space_snapshot(), bytes.m_size);

        Bytes buffer {
            m_buffer + m_producer_offset % sizeof(m_buffer),
            nwritten,
        };
        bytes.copy_trimmed_to(buffer);

        temporarily_disable_interrupts([&] {
            m_producer_offset += nwritten;

            // We can only mess with the transfer count if it's zero.
            // Otherwise, we can not change it atomically.
            // If it's not zero, that's fine too, because we will receive an interrupt when it reaches zero.
            if (dma_hw->ch[0].transfer_count == 0)
                dma_hw->ch[0].transfer_count = m_producer_offset - consumer_offset_snapshot();
        });

        return nwritten;
    }

    void UartOutputDriver::interrupt_end_of_transfer() {
        // FIXME

        // FIXME: We must check if 'transfer_count == 0'.
    }
}
