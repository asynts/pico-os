#pragma once

namespace Kernel {
    class SerialPort : public Singleton<SerialPort> {
    public:
        SerialPort();

        Result<size_t> read(Bytes);
        Result<size_t> write(ReadonlyBytes);
    };
}
