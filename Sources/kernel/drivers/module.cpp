export module kernel.drivers;

export import workaround;

import kit;

using namespace kit;

namespace kernel::drivers
{
    export
    struct UartDriver : public Singleton<UartDriver> {
    private:
        friend Singleton<UartDriver>;
        UartDriver() = default;
    };
}
