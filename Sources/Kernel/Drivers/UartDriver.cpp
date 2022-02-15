#include <Kit/Forward.hpp>

#include <Kernel/Drivers/UartDriver.hpp>

#include <hardware/structs/uart.h>
#include <hardware/structs/dma.h>

// FIXME: Verify that we don't accidently trigger the channel.
// FIXME: verify that all race conditions have been resolved.

namespace Kernel::Drivers
{
    UartDriver::UartDriver()
    {
        // The idea is as follows:
        //
        // -   We configure UART0 to emit DREQ0 when data is avaliable.
        //
        // -   We configure DMA_CHANNEL0 to read from UART0 on DREQ0.
        //     On start-up, the write address is set to the start of the buffer.
        //
        // -   When the buffer is full we get and interrupt and we recalculate the transmission count.
        //     The hope is, that some of the input has already been consumed and is no longer taking up space.
        //
        // -   There is the obvious edge case where the buffer is completely full and we have no way of dealing with the
        //     interrupt.

        // Invariants:
        //
        //  1. The consumer can't be ahead of the producer:
        //
        //     m_consumer_offset <= producer_offset()
        //
        //  2. The producer can't overflow the buffer:
        //
        //     sizeof(buffer) >= producer_offset() - m_consumer_offset
        //
        //  3. Only 'interrupt_dma_complete' can only execute once in parallel.
    }

    usize UartDriver::producer_offset()
    {
        // FIXME: Race: an overflow could occur between the reads.
        return m_producer_offset_base + (*dma_channel0_write_addr - reinterpret_cast<usize>(m_buffer));
    }

    usize UartDriver::bytes_avaliable_for_read()
    {
        // FIXME: Race: more could be produced and consumed between the reads.
        return producer_offset() - m_consumer_offset;
    }

    usize UartDriver::bytes_fitting_in_buffer()
    {
        return sizeof(m_buffer) - (producer_offset() - m_consumer_offset);
    }

    // This means, that the DMA channel finished the transaction.
    // We need to update the transaction count, hopefully somebody read something.
    void UartDriver::interrupt_dma_complete()
    {
        uptr old_write_addr = *dma_channel0_write_addr;
        uptr new_write_addr = reinterpret_cast<uptr>(m_buffer) + producer_offset() % sizeof(m_buffer);

        // If we wrap around, we need to update 'm_producer_offset_base'.
        if (new_write_addr < old_write_addr) {
            m_producer_offset_base += old_write_addr - new_write_addr;
        }

        // FIXME: Race: If somebody reads 'm_producer_offset_base' here, they won't see new new write address.

        *dma_channel0_write_addr = new_write_addr;

        if (bytes_fitting_in_buffer() == 0) {
            // To address this issue, we need to be able restart the transaction when we consume data.
            FIXME();
        }

        // Trigger.
        *dma_channel0_transaction_count_trigger = bytes_fitting_in_buffer();
    }

    void UartDriver::interrupt_uart_full()
    {
        // This means that something is broken, or the buffer is simply to small and the kernel
        // can't keep up.
        FIXME();
    }

    // Configure UART0 to emit DREQ0.
    // The settings here must match with what we configure on the host.
    void UartDriver::configure_uart()
    {
        // UARTCLK = 125 MHz

        // Configure baud rate of 115200 Hz.
        *uart0_integer_baudrate = 67;
        *uart0_fractional_baudrate = 52;

        // Configure wire format.
        UartLineControl line_control = *uart0_line_control;
        line_control.send_break = 0;            // Don't take a break; this should never be 1.
        line_control.parity_enabled = 0;        // Disable parity bit.
        line_control.two_stop_bit_select = 0;   // Only send a single stop bit.
        line_control.fifo_enabled = 1;          // Use FIFO for safety.
        line_control.word_length = 3;           // 8-bit words.
        line_control.stick_parity_select = 0;   // No effect.
        *uart0_line_control = line_control;

        // Configure chip.
        UartControl control = *uart0_control;
        control.uart_enabled = 1;               // Enable the chip.
        control.sir_enabled = 0;                // FIXME
        control.sir_low_power = 0;              // FIXME
        control.loopback_enabled = 0;           // FIXME
        control.transmit_enabled = 1;           // Enable transmitter.
        control.receive_enabled = 1;            // Enable receiver.
        control.request_to_send = 0;            // FIXME
        control.output_1 = 0;                   // FIXME
        control.output_2 = 0;                   // FIXME
        control.rts_flow_control_enabled = 0;   // FIXME
        control.cts_flow_control_enabled = 0;   // FIXME
        *uart0_control = control;

        // Configure DMA.
        UartDmaControl dma_control = *uart0_dma_control;
        dma_control.receive_dma_enabled = 1;    // Enable DMA for receiver.
        dma_control.transmit_dma_enabled = 0;   // Disable DMA for transmitter.
        *uart0_dma_control = dma_control;

        // FIXME: It would be better to keep the interrupts for errors at least.

        // Mask interrupts.
        UartInterruptMask uart_interrupt_mask = *uart0_interrupt_mask;
        uart_interrupt_mask._unknown_1 = 0;     // Disable all interrupts.
        *uart0_interrupt_mask = uart_interrupt_mask;

        // FIXME: Where is DREQ0 programmed?
    }

    // Configure DMA_CHANNEL0 to copy from UART0 on DREQ0.
    // The underlying buffer will be drained by the kernel in parallel.
    void UartDriver::configure_dma()
    {
        // First, we verify that the channel isn't enabled at the moment.
        ASSERT(dma_channel0_ctrl->enabled == 0);

        // Read from UART0.
        *dma_channel0_read_addr = UART0_BASE + UART_DATA;

        // Write into a buffer in memory.
        *dma_channel0_write_addr = reinterpret_cast<uptr>(m_buffer);
        *dma_channel0_transaction_count = sizeof(m_buffer);

        DmaCtrl control = *dma_channel0_ctrl;
        control.enabled = 1;                    // This channel is enabled and will be triggered when we write this back.
        control.high_priority = 1;              // This channel has a high priority compared to other channels.
        control.data_size = 0;                  // Copy 1 byte with each transfer.
        control.increment_read = 0;             // Constant read address.
        control.increment_write = 1;            // Increment write address.
        control.ring_size = 0;                  // Do not wrap.
        control.ring_select = 0;                // No effect.
        control.chain_to = 0;                   // CHANNEL0; Same channel, do not chain.
        control.transfer_request_select = 0;    // DREQ0
        control.irq_quiet = 0;                  // We would like to get an interrupt when the transaction is complete.
        control.byte_swap = 0;                  // No effect.
        control.sniff_enabled = 0;              // We don't want to compute a CRC32 of this transaction.
        control.busy = 0;                       // No effect.
        control.write_error = 1;                // Clear errors.
        control.read_error = 1;                 // Clear errors.
        control.error = 0;                      // No effect.

        // Trigger.
        *dma_channel0_ctrl_trigger = control;
    }
}
