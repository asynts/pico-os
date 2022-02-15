#include <Kit/Assertions.hpp>

#include <Kernel/Drivers/UartOutputDriver.hpp>

namespace Kernel
{
    using namespace Kit;

    inline void entry() {
        Drivers::UartOutputDriver::initialize();
        Drivers::UartOutputDriver::the().write("Hello, world!\n"_b);

        BREAKPOINT();
    }
}
