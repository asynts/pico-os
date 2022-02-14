#include <Kit/Forward.hpp>
#include <Kit/Singleton.hpp>

namespace Kernel::Drivers
{
    using namespace Kit;

    // For the time being, we only deal with reading from UART.
    struct UartDriver : Singleton<UartDriver> {
    private:
    /*
        // When an overflow occurs, we increment this counter.
        // By checking if the overflow interrupt is identical, one can ensure that no overflow occured.
        volatile usize m_overflow_interrupt_counter = 0;
    */

        // FIXME: Get this from the page allocator.
        u8 m_buffer[1 * KiB];
        usize m_consumer_offset = 0;
        usize m_producer_offset_base = 0;

        friend Singleton<UartDriver>;
        UartDriver();

        usize producer_offset();
        usize bytes_avaliable_for_read();
        usize bytes_fitting_in_buffer();

        void configure_uart();
        void configure_dma();
    public:

        void interrupt_dma_complete();
        void interrupt_uart_full();
    };
}
