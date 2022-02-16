#include <Kit/Assertions.hpp>

#include <Kernel/Interrupt.hpp>
#include <Kernel/Drivers/UartOutputDriver.hpp>

namespace Kernel
{
    using namespace Kit;

    inline void entry() {
        Drivers::UartOutputDriver::initialize();
        Drivers::UartOutputDriver::the().try_write("Hello, world!\n"_b);
    }
}
