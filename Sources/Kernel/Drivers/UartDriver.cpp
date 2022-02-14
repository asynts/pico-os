#include <Kit/Forward.hpp>

#include <Kernel/Drivers/UartDriver.hpp>

// FIXME: Move this into 'Kernel/Hardware/*'.

#define UART0_BASE                  0x40034000
#define UART_DATA                        0x000
#define UART_CONTROL                     0x030

#define DMA_BASE                                0x50000000
#define DMA_CHANNEL0_CTRL                            0x010
#define DMA_CHANNEL0_CTRL_TRIGGER                    0x00c
#define DMA_CHANNEL0_READ_ADDR                       0x000
#define DMA_CHANNEL0_WRITE_ADDR                      0x004
#define DMA_CHANNEL0_TRANSACTION_COUNT               0x008
#define DMA_CHANNEL0_TRANSACTION_COUNT_TRIGGER       0x01c

struct DmaCtrl {
    u32 enabled : 1;
    u32 high_priority : 1;
    u32 data_size : 2;
    u32 increment_read : 1;
    u32 increment_write : 1;
    u32 ring_size : 4;
    u32 ring_select : 1;
    u32 chain_to : 4;
    u32 transfer_request_select : 6;
    u32 irq_quiet : 1;
    u32 byte_swap : 1;
    u32 sniff_enabled : 1;
    u32 busy : 1;
    u32 _reserved : 4;
    u32 write_error : 1;
    u32 read_error : 1;
    u32 error : 1;
};
static_assert(sizeof(DmaCtrl) == 4);

DmaCtrl *dma_channel0_ctrl = reinterpret_cast<DmaCtrl*>(DMA_BASE + DMA_CHANNEL0_CTRL);
DmaCtrl *dma_channel0_ctrl_trigger = reinterpret_cast<DmaCtrl*>(DMA_BASE + DMA_CHANNEL0_CTRL_TRIGGER);

// FIXME: Get rid of many 'reinterpret_cast' calls by changing this to 'u8**'.
uptr    *dma_channel0_read_addr = reinterpret_cast<uptr*>(DMA_BASE + DMA_CHANNEL0_READ_ADDR);
// FIXME: Get rid of many 'reinterpret_cast' calls by changing this to 'u8**'.
uptr    *dma_channel0_write_addr = reinterpret_cast<uptr*>(DMA_BASE + DMA_CHANNEL0_WRITE_ADDR);

usize   *dma_channel0_transaction_count = reinterpret_cast<usize*>(DMA_BASE + DMA_CHANNEL0_TRANSACTION_COUNT);
usize   *dma_channel0_transaction_count_trigger = reinterpret_cast<usize*>(DMA_BASE + DMA_CHANNEL0_TRANSACTION_COUNT_TRIGGER);

// FIXME: Verify that we don't accidently trigger the channel.

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
        // FIXME
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
