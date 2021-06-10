#include <Kernel/Interrupt/UART.hpp>

#include <hardware/irq.h>
#include <hardware/uart.h>
#include <hardware/gpio.h>

namespace Kernel::Interrupt
{
    UART::UART()
    {
        uart_init(uart0, 115200);

        gpio_set_function(PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART);

        // FIXME: There seems thre is an initial "junk" byte read, I've seen
        //        0xff and 0xfc
        uart_getc(uart0);

        uart_set_fifo_enabled(uart0, false);

        irq_set_exclusive_handler(UART0_IRQ, UART::interrupt);
        irq_set_enabled(UART0_IRQ, true);

        uart_set_irq_enables(uart0, true, false);
    }

    void UART::interrupt()
    {
        dbgln("[UART::interrupt]");

        while (uart_is_readable(uart0))
        {
            UART::the().m_input_queue.enqueue(uart_getc(uart0));
        }
    }

    KernelResult<usize> UART::read(Bytes bytes)
    {
        usize index;
        for (index = 0; index < min(bytes.size(), m_input_queue.size()); ++index) {
            // FIXME: This is not safe
            bytes[index] = m_input_queue.dequeue();
        }

        return index;
    }

    KernelResult<usize> UART::write(ReadonlyBytes bytes)
    {
        usize index = 0;
        while (uart_is_writable(uart0)) {
            uart_putc_raw(uart0, static_cast<char>(bytes[index]));
            ++index;
        }

        return index;
    }
}
