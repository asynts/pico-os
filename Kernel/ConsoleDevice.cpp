#include <Kernel/ConsoleDevice.hpp>

#include <hardware/uart.h>
#include <hardware/gpio.h>

namespace Kernel
{
    ConsoleFile::ConsoleFile()
    {
        uart_init(uart0, 115200);

        gpio_set_function(PICO_DEFAULT_UART_TX_PIN, GPIO_FUNC_UART);
        gpio_set_function(PICO_DEFAULT_UART_RX_PIN, GPIO_FUNC_UART);

        // FIXME: There seems thre is an initial "junk" byte read, I've seen
        //        0xff and 0xfc
        uart_getc(uart0);
    }

    KernelResult<usize> ConsoleFileHandle::read(Bytes bytes)
    {
        usize nread;
        for (nread = 0; nread < bytes.size(); ++nread)
            bytes[nread] = uart_getc(uart0);

        return nread;
    }

    KernelResult<usize> ConsoleFileHandle::write(ReadonlyBytes bytes)
    {
        usize nwritten;
        for (nwritten = 0; nwritten < bytes.size(); ++nwritten)
            uart_putc_raw(uart0, bytes[nwritten]);

        return nwritten;
    }
}
