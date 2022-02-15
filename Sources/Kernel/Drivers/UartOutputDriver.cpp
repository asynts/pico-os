#include <Kernel/Drivers/UartOutputDriver.hpp>

#include <hardware/structs/dma.h>

// CLAIM: UART0
// CLAIM: DMA_CHANNEL0

// FIXME: Add some interrupt guard helper.
// FIXME: Consider using 'temporarily_disable_interrupts' more.
// FIXME: Consistency: transfer, transmission, etc.

// It could happen, that this function is called in a nested context.
// In that case we need to be careful that we don't re-enable interrupts too early.
//
// -   We do not need to syncronize with the other core, because interrupts are enabled or
//     disabled for each core seperately.
//
// -   We do not need to synchronize with other threads, because when we mask interrupts, the scheduler
//     interrupt is masked as well.
template<typename T>
void ensure_interrupts_are_disabled_in_this_section(T&& callback)
{
    u32 primask;

    // FIXME: Test that this actually works.

    // FIXME: Can we get a scheduler interrupt between these two instructions?
    asm volatile ("mrs %[primask], primask;"
                  "cpsid i;"
        : [primask] "=r"(primask));

    callback();

    asm volatile ("msr primask, %[primask];"
                  "isb;"
        :
        : [primask] "r"(primask));
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

        ensure_interrupts_are_disabled_in_this_section([&] {
            m_producer_offset += nwritten;
            try_update_transfer_count();
        });

        return nwritten;
    }

    void UartOutputDriver::interrupt_end_of_transfer() {
        try_update_transfer_count();
    }
}
