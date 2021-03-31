#pragma once

#include <Std/Result.hpp>

namespace Kernel
{
    using namespace Std;

    enum class KernelError {
    };

    template<typename T>
    class KernelResult : public Result<T, KernelError> {
    public:
        using Result<T, KernelError>::Result;
    };
}
