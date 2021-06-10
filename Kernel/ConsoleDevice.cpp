#include <Kernel/ConsoleDevice.hpp>
#include <Kernel/Interrupt/UART.hpp>

namespace Kernel
{
    ConsoleFile::ConsoleFile()
    {
        Interrupt::UART::the();
    }

    VirtualFile& ConsoleFileHandle::file() { return ConsoleFile::the(); }

    KernelResult<usize> ConsoleFileHandle::read(Bytes bytes)
    {
        // FIXME: If we can't read input here, block the thread and do something else
        for (;;) {
            usize nread = Interrupt::UART::the().read(bytes).must();

            if (nread > 0)
                return nread;
        }
    }

    KernelResult<usize> ConsoleFileHandle::write(ReadonlyBytes bytes)
    {
        // FIXME: If we can't write here, it MIGHT be sensible to block the thread and do something else
        usize nwritten = 0;
        while (bytes.size() > nwritten) {
            nwritten += Interrupt::UART::the().write(bytes.slice(nwritten)).must();
        }

        return nwritten;
    }
}
