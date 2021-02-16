#pragma once

namespace Kernel {
    class SerialDevice : public Device<SerialPort> {
    public:
        SerialDevice();

        Result<usize> read(Bytes);
        Result<usize> write(ReadonlyBytes);
    };
}
