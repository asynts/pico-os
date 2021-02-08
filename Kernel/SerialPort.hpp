#pragma once

namespace Kernel {
    class SerialPort : public Singleton<SerialPort> {
    public:
        SerialPort();

        size_t read(Bytes);
        size_t write(ReadonlyBytes);
    };
}
