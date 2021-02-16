#pragma once

namespace Kernel {
    class SerialPort : public Singleton<SerialPort> {
    public:
        SerialPort();

        Result<usize> read(Bytes);
        Result<usize> write(ReadonlyBytes);
    };
}
