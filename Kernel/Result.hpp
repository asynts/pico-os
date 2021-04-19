#pragma once

#include <Std/Result.hpp>

namespace Kernel
{
    using namespace Std;

    // FIXME: I would like to have errors in an enum, sounds like something for <Kernel/Interface/Errors.hpp>

    template<typename T>
    using KernelResult = Result<T, int>;
}
