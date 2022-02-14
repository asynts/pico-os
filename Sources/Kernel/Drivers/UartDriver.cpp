#include <Kit/Forward.hpp>

#include <Kernel/Drivers/UartDriver.hpp>

#define UART0_BASE                  0x40034000
#define UART_DATA                        0x000
#define UART_CONTROL                     0x030

#define DMA_BASE                    0x50000000
#define DMA_CHANNEL0_CTRL_TRIGGER        0x00c
#define DMA_CHANNEL0_READ_ADDR           0x000
#define DMA_CHANNEL0_WRITE_ADDR          0x004
#define DMA_CHANNEL0_TRANSACTION_COUNT   0x008

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

DmaCtrl *dma_channel0_ctrl_trigger = reinterpret_cast<DmaCtrl*>(DMA_BASE + DMA_CHANNEL0_CTRL_TRIGGER);
uptr    *dma_channel0_read_addr = reinterpret_cast<uptr*>(DMA_BASE + DMA_CHANNEL0_READ_ADDR);
uptr    *dma_channel0_write_addr = reinterpret_cast<uptr*>(DMA_BASE + DMA_CHANNEL0_WRITE_ADDR);
usize   *dma_channel0_transaction_count = reinterpret_cast<usize*>(DMA_BASE + DMA_CHANNEL0_TRANSACTION_COUNT);

namespace Kernel::Drivers
{
    // FIXME: Get this from the page allocator.
    alignas(1 * KiB)
    static u8 uart_buffer[1 * KiB];

    UartDriver::UartDriver()
    {
        configure_dma();
    }

    void UartDriver::configure_dma()
    {
        // We want to configure DMA_CHANNEL0 to copy from UART0 into a circular buffer.
        // The same buffer can be drained by the kernel.
        //
        // Obviously, we can only read data when there is some, thus we use the DREQ feature
        // where the UART chip will tell the DMA chip that data is avaliable.

        // First, we verify that the channel isn't enabled at the moment.
        DmaCtrl control = *dma_channel0_ctrl_trigger;
        ASSERT(control.enabled == 0);

        // This address won't be incremented, we always read from the same location.
        *dma_channel0_read_addr = UART0_BASE + UART_DATA;

        // This address will be incremented and wraps around at 1 KiB.
        *dma_channel0_write_addr = reinterpret_cast<uptr>(uart_buffer);

        // It doesn't matter what we specify here, but this needs to be large.
        // For each byte transmitted this value will be decremented and we receive an interrupt when it reaches zero.
        // In theory, we could then increment the transaction count and go again.
        *dma_channel0_transaction_count = 2 * GiB;

        control.enabled = 1;                    // This channel is enabled and will be triggered when we write this back.
        control.high_priority = 1;              // This channel has a high priority compared to other channels.
        control.data_size = 0;                  // Copy 1 byte with each transfer.
        control.increment_read = 0;             // Increment read address.
        control.increment_write = 1;            // Increment write address.
        control.ring_size = 10;                 // Wrap address at 2^10 = 1 KiB.
        control.ring_select = 1;                // Apply wrapping to write address.
        control.chain_to = 0;                   // CHANNEL0
        control.transfer_request_select = 0;    // DREQ0
        control.irq_quiet = 0;                  // We would like to get an interrupt when the transaction is "complete".
        control.byte_swap = 0;                  // No effect.
        control.sniff_enabled = 0;              // We don't want to compute a CRC32 of this transaction.
        control.busy = 0;                       // No effect.
        control.write_error = 1;                // Clear errors.
        control.read_error = 1;                 // Clear errors.
        control.error = 0;                      // No effect.

        // This will start the transaction.
        *dma_channel0_ctrl_trigger = control;
    }
}
