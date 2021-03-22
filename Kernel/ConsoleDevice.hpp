#pragma once

#include <Std/Span.hpp>
#include <Std/Map.hpp>
#include <Std/Singleton.hpp>

#include <hardware/uart.h>

namespace Kernel {
    using namespace Std;

    class Device {
    public:
        Device(u16 major, u16 minor)
            : m_minor(minor)
            , m_major(major)
        {
            m_devices.append(device_id(), this);
        }

        u32 device_id() { return m_major << 16 | m_minor; }

        virtual usize read(Bytes) = 0;
        virtual usize write(ReadonlyBytes) = 0;

        static Device* lookup(u16 minor, u16 major)
        {
            return lookup(major << 16 | minor);
        }
        static Device* lookup(u32 id)
        {
            return m_devices.lookup(id).value_or(nullptr);
        }

        u16 major() { return m_major; }
        u16 minor() { return m_minor; }

    private:
        static Map<u32, Device*> m_devices;

        u16 m_minor;
        u16 m_major;
    };

    class ConsoleDevice : public Device, public Singleton<ConsoleDevice> {
    public:
        usize read(Bytes bytes)
        {
            isize nread;
            for (nread = 0; nread < bytes.size(); ++nread)
                bytes[nread] = uart_getc(uart0);

            return nread;
        }

        usize write(ReadonlyBytes bytes)
        {
            uart_write_blocking(uart0, bytes.data(), bytes.size());
            return bytes.size();
        }

    private:
        friend Singleton<ConsoleDevice>;
        ConsoleDevice();
    };
}
