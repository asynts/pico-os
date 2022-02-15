#include <Kit/Forward.hpp>
#include <Kit/Span.hpp>
#include <Kit/Singleton.hpp>

namespace Kernel::Drivers
{
    using namespace Kit;

    // This is essentially a circular queue where the kernel produces and the UART consumes.
    // In order to feed the data to the UART, we use a DMA channel.
    //
    // There are three things that can occur in parallel:
    //
    // -   The kernel can produce data that is written into the buffer.
    //
    // -   The UART chip can write data and decrement it's transation count.
    //
    // -   The UART can fire an interrupt when the transaction is complete.
    //
    // Our main goal is to synchronize the kernel with the interrupt handler.
    // In order to ensure this, we assume that both are always executing on core0.
    // This assumption will break when we introduce multiple cores, however, it should be
    // possible to fix that by introducing something in the scheduler.
    //
    // -   The interrupt handler can safely assume that no other code is running except for the UART itself.
    //     This does not hurt, because the interrupt is triggered when the transmission has been completed.
    //
    // -   The kernel always checks the interrupt counter to verify that no interrupt occured and retries if necessary.
    struct UartOutputDriver : Singleton<UartOutputDriver> {
    private:
        u8 m_buffer[1 * KiB];

        usize m_producer_offset = 0;
        usize m_consumer_offset_base = 0;

        // Sometimes, we need to read several values atomically.
        // From the interrupt handler this is fine, but outside of the interrupt handler we need to be careful.
        // We can detect interrupts by caching this value before trying to do some atomic operation.
        // This assumes that no 2^32 interrupts occur.
        volatile usize m_interrupt_counter = 0;

        friend Singleton<UartOutputDriver>;
        UartOutputDriver();

    public:
        void write(ReadonlyBytes);

        void interrupt_end_of_transmission();
    };
}
