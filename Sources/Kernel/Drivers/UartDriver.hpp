#include <Kit/Forward.hpp>
#include <Kit/Singleton.hpp>

namespace Kernel::Drivers
{
    using namespace Kit;

    struct UartDriver : Singleton<UartDriver> {
    private:
        friend Singleton<UartDriver>;
        UartDriver() = default;
    };
}
