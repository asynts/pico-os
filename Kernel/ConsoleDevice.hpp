#pragma once

#include <Std/Span.hpp>
#include <Std/Map.hpp>
#include <Std/Singleton.hpp>

#include <hardware/uart.h>

namespace Kernel {
    using namespace Std;

    class Device {
    public:
        Device(u16 minor, u16 major)
            : m_minor(minor)
            , m_major(major)
        {
        }

        virtual isize read(Bytes) = 0;
        virtual isize write(ReadonlyBytes) = 0;

        Device* lookup(u16 minor, u16 major)
        {
            return m_devices.lookup(major << 16 | minor).value_or(nullptr);
        }

    private:
        static Map<u32, Device*> m_devices;

        u16 m_minor;
        u16 m_major;
    };

    class ConsoleDevice : public Device, public Singleton<ConsoleDevice> {
    public:
        isize read(Bytes bytes)
        {
            isize nread;
            for (nread = 0; nread < bytes.size(); ++nread)
                bytes[nread] = uart_getc(uart0);

            return nread;
        }

        isize write(ReadonlyBytes bytes)
        {
            uart_write_blocking(uart0, bytes.data(), bytes.size());
            return bytes.size();
        }

    private:
        friend Singleton<ConsoleDevice>;
        ConsoleDevice()
            : Device(1, 0)
        {
        }
    };
}
