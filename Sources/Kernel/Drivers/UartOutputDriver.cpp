#include <Kernel/Interrupt.hpp>
#include <Kernel/Drivers/UartOutputDriver.hpp>

#include <hardware/structs/dma.h>

// CLAIM: UART0
// CLAIM: DMA_CHANNEL0

namespace Kernel::Drivers
{
    u8 target_buffer[4 * KiB];

    // FIXME: Somehow, we go through this constructor twice.
    //        I suspect, that this is once visited by CORE0 and then by CORE1.
    //        However, the 'm_initialized' in Singleton should prevent hat.
    UartOutputDriver::UartOutputDriver() {
        // FIXME: Fill the buffer with gibberish to be able to detect it easier.
        for (usize i = 0; i < sizeof(m_buffer); ++i) {
            m_buffer[i] = 0b00111100;
        }

        // Configure DMA_CHANNEL0 to read from the buffer and write to our target buffer.
        dma_hw->ch[0].read_addr = reinterpret_cast<uptr>(m_buffer);
        dma_hw->ch[0].write_addr = reinterpret_cast<uptr>(target_buffer);
        dma_hw->ch[0].transfer_count = 0;
        dma_hw->ch[0].ctrl_trig = DMA_CH0_CTRL_TRIG_EN_BITS
                                | DMA_CH0_CTRL_TRIG_HIGH_PRIORITY_RESET     // Low priority.
                                | (0 << DMA_CH0_CTRL_TRIG_DATA_SIZE_LSB)    // Transfer individual bytes.
                                | DMA_CH0_CTRL_TRIG_INCR_READ_BITS          // Increment read address.
                                | DMA_CH0_CTRL_TRIG_INCR_WRITE_BITS         // FIXME: Increment write address to debug functionality.
                                | (0 << DMA_CH0_CTRL_TRIG_RING_SIZE_LSB)    // Do not wrap around.
                                | (0 << DMA_CH0_CTRL_TRIG_CHAIN_TO_LSB)     // Same channel means no chaining.
                                | (0x3f << DMA_CH0_CTRL_TRIG_TREQ_SEL_LSB)  // FIXME: Permanent transfer, do not wait for peripheral.
                                | DMA_CH0_CTRL_TRIG_IRQ_QUIET_RESET         // Receive interrupt when transfer complete.
                                | DMA_CH0_CTRL_TRIG_SNIFF_EN_RESET          // Do not compute a CRC32 from this stream.
                                | DMA_CH0_CTRL_TRIG_WRITE_ERROR_RESET       // Clear errors.
                                | DMA_CH0_CTRL_TRIG_READ_ERROR_RESET        // Clear errors.
                                ;

        // FIXME: Configure UART0.
    }

    // If the DMA is currently doing something, this will return a stale value.
    // This is fine, but the caller needs to be aware of this.
    usize UartOutputDriver::consumer_offset_snapshot() {
        usize result;
        with_interrupts_disabled([&] {
            result = m_consumer_offset_base + (dma_hw->ch[0].read_addr - reinterpret_cast<usize>(m_buffer));
        });
        return result;
    }

    // If the DMA is currently doing something, this will return a stale value.
    // This is fine, but the caller needs to be aware of this.
    usize UartOutputDriver::avaliable_space_snapshot() {
        return sizeof(m_buffer) - (m_producer_offset - consumer_offset_snapshot());
    }

    // We can only mess with the transfer count if it's zero.
    // Otherwise, we can not change it atomically.
    //
    // If it is not zero, that is fine because we will receive an interrupt when it reaches zero.
    // If we are in an interrupt handler this is fine too, because one interrupt will be buffered.
    void UartOutputDriver::try_update_transfer_count() {
        if (dma_hw->ch[0].transfer_count == 0) {
            usize consumer_offset = consumer_offset_snapshot();

            dma_hw->ch[0].read_addr = reinterpret_cast<uptr>(m_buffer) + (consumer_offset % sizeof(m_buffer));
            dma_hw->ch[0].transfer_count = m_producer_offset - consumer_offset;
        }
    }

    // We write as many bytes as we can safely fit into the buffer.
    // If that isn't what the caller asked for, he should try again later.
    // It is possible that 'try_write' returns zero.
    usize UartOutputDriver::try_write(ReadonlyBytes bytes) {
        usize nwritten = min(avaliable_space_snapshot(), bytes.m_size);

        Bytes buffer {
            m_buffer + m_producer_offset % sizeof(m_buffer),
            nwritten,
        };
        bytes.copy_trimmed_to(buffer);

        with_interrupts_disabled([&] {
            m_producer_offset += nwritten;
            try_update_transfer_count();
        });

        return nwritten;
    }

    void UartOutputDriver::interrupt_end_of_transfer() {
        m_interrupt_counter = m_interrupt_counter + 1;
        try_update_transfer_count();
    }
}
