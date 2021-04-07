#pragma once

#include <hardware/uart.h>

#include <Std/Span.hpp>
#include <Std/Singleton.hpp>

#include <Kernel/FileSystem/DeviceFileSystem.hpp>

namespace Kernel {
    using namespace Std;

    class ConsoleFileHandle final : public VirtualFileHandle
    {
    public:
        KernelResult<usize> read(Bytes bytes) override
        {
            usize nread;
            for (nread = 0; nread < bytes.size(); ++nread)
                bytes[nread] = uart_getc(uart0);

            return nread;
        }

        KernelResult<usize> write(ReadonlyBytes bytes) override
        {
            uart_write_blocking(uart0, bytes.data(), bytes.size());
            return bytes.size();
        }
    };

    class ConsoleFile final
        : public Singleton<ConsoleFile>
        , public VirtualFile
    {
    public:
        VirtualFileHandle& create_handle() override
        {
            return *new ConsoleFileHandle;
        }

    private:
        friend Singleton<ConsoleFile>;
        ConsoleFile() = default;
    };
}
