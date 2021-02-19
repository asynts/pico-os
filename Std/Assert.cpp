#include <Std/Assert.hpp>
#include <Kernel/DebugDevice.hpp>

namespace Std {
    void crash(const char *filename, usize line)
    {
        Kernel::DebugDevice::the().crash(filename, line);
    }

    void assert_that(bool condition, const char *filename, usize line)
    {
        if (!condition)
            crash(filename, line);
    }
}
