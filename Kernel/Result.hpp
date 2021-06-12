#pragma once

#include <Std/Result.hpp>

#include <Kernel/Forward.hpp>

namespace Kernel
{
    // FIXME: I would like to have errors in an enum, sounds like something for <Kernel/Interface/Errors.hpp>

    template<typename T>
    using KernelResult = Result<T, int>;
}
