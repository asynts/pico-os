#pragma once

#include <Std/Singleton.hpp>

namespace Kernel
{
    using namespace Std;

    class Scheduler : public Singleton<Scheduler> {
    public:
        Scheduler();
    };
}
