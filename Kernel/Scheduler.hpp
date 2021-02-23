#pragma once

#include <Std/Singleton.hpp>
#include <Std/Span.hpp>
#include <Std/Vector.hpp>

namespace Kernel
{
    using namespace Std;

    struct Task {
        u8 *top_of_stack;
    };

    class Scheduler : public Singleton<Scheduler> {
    public:
        Scheduler();
    };
}
