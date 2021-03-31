#pragma once

#include <hardware/uart.h>

#include <Std/Span.hpp>
#include <Std/Map.hpp>
#include <Std/Singleton.hpp>

#include <Kernel/FileSystem/DeviceFileSystem.hpp>

namespace Kernel {
    using namespace Std;

    class ConsoleFileHandle final : public VirtualFileHandle
    {
    public:
        VirtualFile& file() override { return *m_file; }

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

        VirtualFile *m_file;
    };

    class ConsoleFile final
        : public Singleton<ConsoleFile>
        , public VirtualFile
    {
    public:
        VirtualFileSystem& filesystem() override { return DeviceFileSystem::the(); }

        VirtualFileHandle& create_handle() override
        {
            auto& handle = *new ConsoleFileHandle;
            handle.m_file = this;
            return handle;
        }

    private:
        friend Singleton<ConsoleFile>;
        ConsoleFile() = default;
    };
}
