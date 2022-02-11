#include <Kit/Assertions.hpp>

#include <Kernel/Drivers/UartDriver.hpp>

namespace Kernel
{
    using namespace Kit;

    inline void entry() {
        Drivers::UartDriver::initialize();

        BREAKPOINT();
    }
}
