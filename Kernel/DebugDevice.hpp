#pragma once

#include <Std/Singleton.hpp>

namespace Kernel {
    using namespace Std;

    class DebugDevice : public Singleton<DebugDevice> {
    public:
        [[noreturn]]
        void crash(const char *filename, usize line)
        {
            for(;;)
                asm("wfi");
        }
    };
}
